FireMerge is a custom tool for merging Cocos Creator layouts. It was origonally written for mecurial, but should work with git and svn.

**Building**:

FireMerge is built with cmake.

On Linux or Mac

```
git clone https://github.com/DavidDeSimone/FireMerge
cd FireMerge/
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

You will need to add the following lines to your .gitattributes

```
**.fire merge=FireMerge
**.prefab merge=FireMerge
```

Add the following to your git config file

```
[merge "FireMerge"]
name = FireMerge
driver = /usr/local/bin/FireMerge %A %O %B
```

OR run

```
git config merge.FireMerge.driver "/usr/local/bin/FireMerge %A %O %B"
```


**Handling Merges**

FireMerge does NODE based merging. This means that it will compare individually identified nodes against each other. If either of these nodes have been touched across branches, FireMerge will report a conflict you will need to manually resolve. Due to CocosCreator's array indexing pattern, conflicted nodes will occupy have the following format

```
    {
        "MINE >>>>>>>>": {
            "__type__": "cc.Node",
            "_active": true,
            "_anchorPoint": {
                "__type__": "cc.Vec2",
                "x": 0.5,
                "y": 0.5
            },
            "_cascadeOpacityEnabled": true,
            "_children": [
                {
                    "__id__": 3
                }
            ],
            "_color": {
                "__type__": "cc.Color",
                "a": 255,
                "b": 255,
                "g": 255,
                "r": 255
            },
            "_components": [
                {
                    "__id__": 4
                }
            ],
            "_contentSize": {
                "__type__": "cc.Size",
                "height": 640,
                "width": 960
            },
            "_globalZOrder": 0,
            "_id": "834f1BEjTBPUZb6xY6a58Mr",
            "_localZOrder": 0,
            "_name": "Canvas",
            "_objFlags": 0,
            "_opacity": 255,
            "_opacityModifyRGB": false,
            "_parent": {
                "__id__": 1
            },
            "_position": {
                "__type__": "cc.Vec2",
                "x": 480,
                "y": 320
            },
            "_prefab": null,
            "_rotationX": 0,
            "_rotationY": 0,
            "_scaleX": 1,
            "_scaleY": 1,
            "_skewX": 0,
            "_skewY": 0,
            "_tag": -1,
            "groupIndex": 0
        },
        "THEIRS <<<<<<<<": {
            "__type__": "cc.Node",
            "_active": true,
            "_anchorPoint": {
                "__type__": "cc.Vec2",
                "x": 0.5,
                "y": 0.5
            },
            "_cascadeOpacityEnabled": true,
            "_children": [
                {
                    "__id__": 5
                }
            ],
            "_color": {
                "__type__": "cc.Color",
                "a": 255,
                "b": 255,
                "g": 255,
                "r": 255
            },
            "_components": [
                {
                    "__id__": 4
                }
            ],
            "_contentSize": {
                "__type__": "cc.Size",
                "height": 640,
                "width": 960
            },
            "_globalZOrder": 0,
            "_id": "834f1BEjTBPUZb6xY6a58Mr",
            "_localZOrder": 0,
            "_name": "Canvas",
            "_objFlags": 0,
            "_opacity": 255,
            "_opacityModifyRGB": false,
            "_parent": {
                "__id__": 1
            },
            "_position": {
                "__type__": "cc.Vec2",
                "x": 480,
                "y": 320
            },
            "_prefab": null,
            "_rotationX": 0,
            "_rotationY": 0,
            "_scaleX": 1,
            "_scaleY": 1,
            "_skewX": 0,
            "_skewY": 0,
            "_tag": -1,
            "groupIndex": 0
        }
    },
```

Here, this conflicted node has been filled with two keys, MINE and THEIRS. It should be replaced with the resulting non-conflicted node. An example output of the merge would be

```
{
            "__type__": "cc.Node",
            "_active": true,
            "_anchorPoint": {
                "__type__": "cc.Vec2",
                "x": 0.5,
                "y": 0.5
            },
            "_cascadeOpacityEnabled": true,
            "_children": [
                {
                    "__id__": 5,
		    "__id__": 3
                }
            ],
            "_color": {
                "__type__": "cc.Color",
                "a": 255,
                "b": 255,
                "g": 255,
                "r": 255
            },
            "_components": [
                {
                    "__id__": 4
                }
            ],
            "_contentSize": {
                "__type__": "cc.Size",
                "height": 640,
                "width": 960
            },
            "_globalZOrder": 0,
            "_id": "834f1BEjTBPUZb6xY6a58Mr",
            "_localZOrder": 0,
            "_name": "Canvas",
            "_objFlags": 0,
            "_opacity": 255,
            "_opacityModifyRGB": false,
            "_parent": {
                "__id__": 1
            },
            "_position": {
                "__type__": "cc.Vec2",
                "x": 480,
                "y": 320
            },
            "_prefab": null,
            "_rotationX": 0,
            "_rotationY": 0,
            "_scaleX": 1,
            "_scaleY": 1,
            "_skewX": 0,
            "_skewY": 0,
            "_tag": -1,
            "groupIndex": 0
        }

```

In our example here, two branches both added a node to a scene. Our resolution was to manually merge their child arrays, adding them together. 