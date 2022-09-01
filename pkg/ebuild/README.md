# Fildesh Gentoo Package

You can find a `fildesh.ebuild` in this directory.
It can be installed manually on Gentoo with the following commands.

```shell
# Start in the project's root directory (2 levels up from here).

# Create "local" repository.
eselect repository create local
# Unmask package.
echo dev-lang/fildesh >>/etc/portage/package.accept_keywords/00_world

# Get latest release version.
fildesh_version=$(grep -m1 -E -e 'version = ' MODULE.bazel | sed -E -e 's/.*"(.*)".*/\1/')
# Make location for ebuild.
install -d /var/db/repos/local/dev-lang/fildesh
# Copy ebuild.
install -T pkg/ebuild/fildesh.ebuild /var/db/repos/local/dev-lang/fildesh/fildesh-${fildesh_version}.ebuild
# Create Manifest entry.
ebuild /var/db/repos/local/dev-lang/fildesh/fildesh-${fildesh_version}.ebuild manifest

# Install.
emerge -a dev-lang/fildesh
```
