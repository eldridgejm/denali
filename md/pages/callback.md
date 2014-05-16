# The Callback System

---

- [Callback system overview](#callback-system-overview)
- [The selection file](#the-selection-file)
    - [`file` section](#file-section)
    - [`component` section](#component-section)
    - [`members` section](#members-section)
    - [`subtree` section](#subtree-section)
    - [An example selection file](#an-example-selection-file)
    - [A note on the deletion of selection
      files](#a-note-on-the-deletion-of-selection-files)
- [Tips and tricks for writing callbacks](#tips-and-tricks-for-writing-callbacks)
    - [Debugging callback scripts](#debugging-callback-scripts)
    - [Non-blocking operation](#non-blocking-operation)
    - [Working with paths](#working-with-paths)

---

## Callback system overview

*Denali* provides a flexible and powerful callback system for invoking processes
upon the user selecting a component of the landscape. Some features of the
system include:

- Callbacks may be written in any language
- Interprocess communication is done via flat file &mdash; there is no
  dependency on a 3rd party messaging library
- Callbacks can print to the status box, and even change the landscape that is
  being visualized.

See the [Printing special information about a
selection](tutorial.html#printing-special-information-about-a-selection)
section of the tutorial for a step-by-step introduction to creating a callback
script. 

There are three types of callbacks. *Info* callbacks print information to the
status box. *Tree* callbacks supply *denali* with a new tree to visualize. And
*async* callbacks are run asynchronously, providing no information to *denali*.

The *info* and *tree* callbacks are necessarily synchronous, meaning, they block
the *denali* process. When these callbacks are invoked, the *denali* interface
will become unresponsive until the callbacks finish executing. 

In the case of *info* and *tree* callbacks, the callback system works as follows:

1. When the callback is invoked, either by the user selecting a component or
   manually invoking the callback by pressing the callback button, a text file
   containing information about the selection is written to disk. This file will
   be referred to as the "selection file".

2. The callback command is run as a blocking subprocess (*denali*'s interface will
   be frozen until the script returns). The process is given as its first
   argument the path to the selection file.

3. The callback script runs. The output it prints to STDOUT is used in different
   ways, depending on the type of the callback:

   - *Info*: the output is printed to the status box
   - *Tree*: the output is interpreted as if it were the content of a `.tree`
   file, and the visualization is updated to represent this tree

    Any other output of the callback that isn't on STDOUT, such as that printed
    to STDERR, is not handled by *denali*.

4. Once the callback has completed, the selection file is automatically deleted
   by *denali*.

In the case of an *async* callback, the system's first step is exactly the same,
steps 2-4 are not. After the selection file is written and the callback is
invoked, *denali* detaches the callback process and has no further interaction
with it. Also, as noted later in (#a-note-on-the-deletion-of-selection-files),
the selection file is **not** deleted when an *async* callback is run. It is the
responsibility of the callback process to delete the selection file. If the
python convenience functions are used, this deletion is automatically done.


## The selection file
*Denali* prints information about the selection to a temporary file. Information
contained in the file includes the selected component, the members contained
within the component, the path to the file being visualized, and (optionally) a
list of all of the nodes and members present in the subtree induced by the
selection.

Note that if you are writing a callback in Python, you *do not* need to know how
to parse the selection file: utilities are included in the *denali.py* module
which read selection files, namely, the `read_selection_file` function. This
section is useful only to users who would like to write their own callbacks in
another language.

The selection file is broken into sections. Each section begins with a line
containing `# ` followed by the section name, i.e:

~~~~
# <section 1 name>
...
# <section 2 name>
...
~~~~

Possible section names are:
`file`, `component`, `members` and `subtree`. The format of each of these
sections is described in detail below:

### `file` section 

**Description**: Contains the full, absolute file path of the tree currently
being visualized.  This path is *not* changed by running a tree callback. In
other words, if we open `/path/to/foo.tree` and run the tree callback,
subsequent selections will still have `/path/to/foo.tree` in the file section
even though the tree being visualized is not the same as the tree described by
this file.

**Format**: A single line containing the full file path to the tree file.

**Example**:

~~~~
# file
/path/to/file.tree
~~~~

### `component` section

**Description**: Contains the ids and scalar values of the two nodes which
define the selected component.

**Format**: Two lines, each with two tab-separated numerical values. The first
value on a line is an integer node id, the second value is a floating-point
scalar value. The first line corresponds to the parent node of the component.
The second corresponds to the child node. For example:

~~~~
# component
<parent id> <parent value>
<child id>  <child value>
~~~~

**Example**:

~~~~
# component
0   42.3
20  -21.1
~~~~

### `members` section

**Description**: Contains a list of all of the members in a selected component
and their associated scalar values. Note that the nodes defining the component
are themselves members of the component by convention. In other words, if the
component 0 → 20 is selected, then 0 and 20 will appear in the `members` section
as well as the `component` section.

**Format**: Same as the `component` section, but with two or more lines (since
at least the nodes defining the component are present). The order of the members
is not important:

~~~
# members
<node id>   <node value>
<node id>   <node value>
<node id>   <node value>
...
~~~

**Example**:

~~~~
# members
1   42.3
10  77.2
42  -4.3
11  99
~~~~

### `subtree` section

**Description**: A list of all of the nodes and members in the subtree induced
by the selection. This is only printed to the selection file if the **Supply
subtree** option is checked for the callback in the **File → Configure
Callbacks** dialog. Otherwise, the section header is not present. By convention,
the selected component is *not* included in the subtree.

**Format**: Same as the `members` section.

**Example**:

~~~~
# subtree
1   42.3
10  77.2
42  -4.3
11  99
~~~~


### An example selection file 

The following selection file was generated by selecting the base component (the
component representing arc 4 → 5) of the tree file located at
`examples/tutorial/tree.tree`. Note that the file is **tab-delimited**.

~~~~
# file
/path/to/denali/examples/tutorial/tree.tree
# component
4	16
5	32
# members
0	25
4	16
5	32
# subtree
1	62
2	45
3	66
6	64
7	39
8	58
9	51
10	53
11	30
~~~~

### A note on the deletion of selection files

Selection files are written where your system keeps its temporary files (for
example, `/tmp` on Unix). If an info or tree callback is invoked, it is possible
to automatically removed the temporary file when the callback completes, and
*denali* does so. It is not known, however, when an *async* callback completes,
so *denali* does not automatically delete the selection file when *async*
callbacks are invoked.  Instead, it is up to the callback process to delete the
file when it no longer needs it.

If you use the python convenience function `read_selection_file`, then you do
not need to do anything special. This function automatically deletes the
selection file after it is read.

If you are using another language, you should remove the selection file after
your callback process reads it. If you neglect to do this, it is unlikely that
anything *bad* will happen -- temporary files are automatically removed by your
operating system every so often. Nevertheless, selection files for large
datasets can themselves be large, and so it is best to clean them.


## Tips and tricks for writing callbacks

Included in this section are several "tricks" for writing callbacks.

### Debugging callback scripts

If your callback encounters a problem and must exit, it will most likely print
the error to STDERR. For example, a python script will print an exception to
STDERR before exiting.

*Denali* does not print the output from STDERR to the status box by design (it
only prints the STDOUT, and that only occurs if the callback is an "info
callback"). Nevertheless, you can still view the error output of your callback
by launching *denali* in a terminal. Anything printed by your callback to STDERR
will be passed along and displayed in the console.

This can be awkward, however. If much debugging needs to be done, a better
alternative is to copy a selection file produced by denali and simply run the
callback from the shell with the path to the selection file as its first
argument. 


### Non-blocking operation

When *denali* runs a callback, it must wait for the callback to finish. This is
because the output of the callback is used to print information to the status
box and/or regenerate the visualization. This isn't always necessary, however.
If you'd prefer that *denali* didn't wait while your callback was running (that
is, that it worked in "non-blocking" fashion), then you should write a quick
script in your favorite language to fork your callback in the background.

Another possibility is to start a *server* script before starting
*denali*, and specify a *client* script as the callback. Whenever a selection is
made, the small client contacts the server, telling it to perform some action.
This is the route taken by the "clustering" example provided alongside *denali*.
Look at example's python code to see how one might do this.

### Working with paths

This is less of a tip, and more of a warning: any callback run by *denali*
inherits *denali*'s environment. This means that if you include any relative
paths in your callback script, you have to be careful about where you start
*denali*. 

Many of the examples avoid this by making the paths relative to the location of
the callback script, so that as long as the script and the files it requires are
moved together, the script functions.

