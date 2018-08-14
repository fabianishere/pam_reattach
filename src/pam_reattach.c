/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Fabian Mastenbroek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define PAM_SM_AUTH
#include <security/pam_modules.h>
#include <security/pam_appl.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "bootstrap.h"
#include "xpc.h"

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	int err;
	char buffer[2 * PATH_MAX];
	const char *username;
	struct passwd *pwd;
	struct passwd pwdbuf;
	uid_t uid, suid;

	/* Get the username (and UID) */
	if ((err = pam_get_user(pamh, &username, NULL)) != PAM_SUCCESS) {
		openpam_log(PAM_LOG_ERROR, "The username could not be obtained: %d %s", err, pam_strerror(pamh, err));
		return PAM_USER_UNKNOWN;
	}
	if (getpwnam_r(username, &pwdbuf, buffer, sizeof(buffer), &pwd) != 0) {
		openpam_log(PAM_LOG_ERROR, "The pwd for %s could not be obtained", username);
		return PAM_USER_UNKNOWN;
	}
	uid = pwd->pw_uid;

	openpam_log(PAM_LOG_DEBUG, "Going to switch to (%s) %u's Aqua bootstrap namespace", username, uid);

	xpc_object_t dict = xpc_dictionary_create(NULL, NULL, 0);

	xpc_dictionary_set_uint64(dict, "subsystem", 0x3);
	xpc_dictionary_set_uint64(dict, "routine", 0x343);
	xpc_dictionary_set_uint64(dict, "handle", 0x0);
	xpc_dictionary_set_uint64(dict, "type", 0x1);
	xpc_dictionary_set_uint64(dict, "uid", uid);

	xpc_object_t reply;
	xpc_pipe_t pipe = xpc_pipe_create_from_port(bootstrap_port, 0);

	/* Obtain the user's Aqua bootstrap port */
	if ((err = xpc_pipe_routine(pipe, dict, &reply)) != 0) {
		openpam_log(PAM_LOG_ERROR, "Failed to obtain Aqua bootstrap port: %d %s", err, strerror(err));

		xpc_release(pipe);
		xpc_release(dict);

		return PAM_SERVICE_ERR;
	} else if ((err = xpc_dictionary_get_int64(reply, "error")) != 0) {
		openpam_log(PAM_LOG_ERROR, "Failed to obtain Aqua bootstrap port: %d %s", err, strerror(err));

		xpc_release(dict);
		xpc_release(pipe);
		xpc_release(reply);

		return PAM_SERVICE_ERR;
	}

	mach_port_t puc = xpc_dictionary_copy_mach_send(reply, "bootstrap");

	/* Attach to the user's per-session bootstrap port */
	mach_port_mod_refs(mach_task_self(), bootstrap_port, MACH_PORT_RIGHT_SEND, -1);
	task_set_bootstrap_port(mach_task_self(), puc);
	mach_port_deallocate(mach_task_self(), bootstrap_port);
	bootstrap_port = puc;

	/* Release the XPC data structures */
	xpc_release(dict);
	xpc_release(pipe);
	xpc_release(reply);

	return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	return PAM_SUCCESS;
}

