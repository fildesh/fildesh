# Releasing a new version on GitHub

## Variables
```shell
RELEASE_TAG=v1.2.3
```

## Procedure
Remember to set [environment variables](#variables) for the commands below.
Also, do some [prework](#prework) on this machine if you haven't before.

### Create the Release

1. Update version number in `MODULE.bazel` and `src/bin/version.h`.
1. Commit that with a message like "Version 1.2.3".
1. Just create the release on GitHub. Let it create a new tag v1.2.3.
   * Copy the previous release notes format.

### Upload Artifacts

1. Push to release branch and get artifact zip from `pkg_release` GitHub wokflow.
1. Unzip `artifact.zip`.
1. Get a classic token.
   * https://github.com/settings/tokens
   * In `Note`, type something like "v1.2.3 release".
   * Select `write:packages`.
1. Export the `GH_TOKEN` environment variable as that token value.
1. Run: `gh release upload "${RELEASE_TAG}" fildesh* -R fildesh/fildesh`.

## Prework

* Install GitHub cli ([instructions](https://github.com/cli/cli/blob/trunk/docs/install_linux.md)).
