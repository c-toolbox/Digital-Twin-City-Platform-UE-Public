# AppDesign Plugin

Add this Plugin to a project by doing the following, if the project is also on the same server. Here we have a project in a sibling group to `common`. For the relative path to work the commands must be done in the root directory of the repo.

```
git submodule add ../../common/AppDesign.git Plugins/AppDesign
git submodule update --init --recursive
```

or this if your project is on a different server.

```
git submodule add git@gitlab.liu.se:Exploranation/common/AppDesign.git Plugins/AppDesign
git submodule update --init --recursive
```

## Setup

This plugin contains C++ code that needs to be compiled after the plugin has been added to a project.

After the plugin has been added and the code compiled you need to create a Data Table that contains the keys and associated fonts to be used by the AppDesign subsystem (when ADText queries for fonts).
See `Content/ExampleFontsTable` for an example of the Data Table.
In Project Settings under Game section, specify the default font and the App Fonts Table.
If you see an error like `LogAppDesign: Error: Failed to load AppFontsTable ''` it means that this data table has not been set.`

## Examples

There are some example tables available and a little Helper Blueprint Library called `AppDesignHelpers`. So far the VEINS project contains some uses of the AppDesign subsystem and the AppDesignTextBlock (Display name `ADText`).

The Lato font is also provided, a free font from Google Fonts.