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

#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>

#define PAM_SM_AUTH
#include <security/pam_modules.h>
#include <security/pam_appl.h>

#include <reattach.h>

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	int err;
	const char *username;
	char buffer[2 * PATH_MAX];
	struct passwd *pwd;
	struct passwd pwdbuf;
	uid_t uid;

	/* Get the username (and UID) */
	if ((err = pam_get_user(pamh, &username, NULL)) != PAM_SUCCESS) {
		openpam_log(PAM_LOG_ERROR, "The username could not be obtained: %d %s", err, pam_strerror(pamh, err));
		return PAM_USER_UNKNOWN;
	}
	if (getpwnam_r(username, &pwdbuf, buffer, sizeof(buffer), &pwd) != 0) {
		openpam_log(PAM_LOG_ERROR, "The password file entry for %s could not be obtained", username);
		return PAM_USER_UNKNOWN;
	}
	uid = pwd->pw_uid;

	openpam_log(PAM_LOG_DEBUG, "Going to switch to (%s) %u's Aqua bootstrap namespace", username, uid);

	/* Reattach to aqua session */
	if ((err = reattach_aqua(uid)) != 0) {
		openpam_log(PAM_LOG_ERROR, "Failed to obtain Aqua bootstrap port: %d %s", err, strerror(err));
		return PAM_SERVICE_ERR;
	}

	return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	return PAM_IGNORE;
}

