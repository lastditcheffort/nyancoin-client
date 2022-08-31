Below find the instructions for building Snapcraft packages. Tested on amd64 & arm64 Linux using Ubuntu 20.04 & 22.04. This package is for those building for themselves and IS NOT yet available in the Snapcraft store.

---

## Building Locally

```
sudo apt install snapd
sudo snap install --classic snapcraft
snapcraft
```

### To Install Locally
```
snap install \*.snap --devmode
```

### To Push to Snapcraft
```
snapcraft login
snapcraft register nyancoin-core
snapcraft push \*.snap --release=edge
sudo snap install nyancoin-core --channel=edge
```
