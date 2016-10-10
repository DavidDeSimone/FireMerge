FireMerge is a custom tool for merging Cocos Creator layouts. It was origonally written for mecurial, but should work with git and svn.

**Building**:

FireMerge is built with cmake.

On Linux or Mac

```
git clone https://github.com/DavidDeSimone/FireMerge
mkdir build && cd build/
cmake ..
make
sudo make install
```

**Usage**:

You will need to set up a custom merge tool in your version control program of choice

*Mecurial*

Add the following lines to your .hgrc

```
[merge-tools]
mergetool.priority = 100
mergetool.premerge = False
mergetool.args = $local $other $base -o $output
FireMerge = /usr/local/bin/FireMerge 

[merge-patterns]
**.fire = /usr/local/bin/FireMerge
**.prefab = /usr/local/bin/FireMerge
```

You can customize the file locations firemerge will look for, or the location of fire merge by editing the merge-patterns.

*Git*

Add the following lines to your git configuarion file:

```
[merge]
tool = FireMerge
[mergetool "FireMerge"]
cmd = /usr/local/bin/FireMerge "$LOCAL" "$BASE" "$REMOTE" "$MERGED"
trustExitCode = true
```