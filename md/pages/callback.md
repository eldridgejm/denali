# The Callback System

---

- [Callback system overview](#callback-system-overview)
- [The selection file](#the-selection-file)
    - [`file` section](#file-section)
    - [`component` section](#component-section)
    - [`members` section](#members-section)
    - [`reduction` section](#reduction-section)
    - [`subtree` section](#subtree-section)
    - [`subtree_reduction` section](#subtree-reduction-section)
    - [An example selection file](#an-example-selection-file)

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

The callback system works as follows:

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
   - *Void*: the output is ignored

    Any other output of the callback that isn't on STDOUT, such as that printed
    to STDERR, is not handled by *denali*.

Because the system communicates via files and STDOUT, callbacks may be written
in virtually any language.

## The selection file
*Denali* prints information about the selection to a temporary file. Information
contained in the file includes the selected component, the members contained
within the component, the path to the file being visualized, and (optionally) a
list of all of the nodes and members present in the subtree induced by the
selection.

Note that if you are writing a callback in Python, you *do not* need to know how
to parse the selection file: utilities are included in the *denali.py* module
which read selection files. This section is useful only to users who would like
to write their own callbacks in another language.

The selection file is broken into sections. Each section begins with a line
containing `# ` followed by the section name, i.e:

~~~~
# <section 1 name>
...
# <section 2 name>
...
~~~~

The `file`, `component`, and `members` sections are always present. The
`reduction` section is present only when a color map has been provided to
denali. The `subtree` section is present only when the "Provide subtree" option
is set in the callback configuration dialog. And the `subtree_reduction` section
is present only when both the "Provide subtree" option is set and a color map
was provided.

We will now described the format and purpose of each of these sections.

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


### `reduction` section

*Only present when a color map was provided to denali.*

**Description**: A color map is provided to denali by specifying a second scalar
value for each of the nodes and members in the tree. As each edge of the tree
has multiple nodes and members associated with it, it is necessary to *reduce* a
set of scalar values to a single number, which is then mapped to a color. Denali
provides several reduction functions, such as the mean and the covariance, for
this task. This section of the file gives the result of the reduction for the
selected arc.

**Format**: A single line containing the result of the reduction as a decimal
number.

**Example**:

~~~~
# reduction
42.0
~~~~

### `subtree` section

*Only present when the "Supply subtree" option is set in the "Configure
Callbacks" dialog.*

**Description:** The subtree induced by the user's selection.

**Format**: The tree is represented using denali `.tree` format (see the [format
specifications](./formats.html#tree)).

**Example**:

This example shows a subtree with 9 nodes.

~~~~
# subtree
9
1	62
3	66
4	16
5	32
7	39
8	58
9	51
10	53
11	30
4	5	0	25
5	1	2	45
5	7
7	8
7	10
7	11
10	3	6	64
10	9
~~~~


### `subtree_reduction` section

*Only present when both the "Supply subtree" option is set in the "Configure
Callbacks" dialog, and a color map was provided to denali.*

**Description**: The result of applying the reduction function to each of the
edges in the subtree.

**Format**: $n$ lines, where $n$ is the number of edges in the subtree, where
each line is of the form:

~~~~
<parent_id> <child_id>  <reduction_value>
~~~~

`<parent_id>` is the id of the parent node of the arc, and `<child_id>` is the
id of the child. The columns are tab-separated, as usual.

**Example:**

This example shows the reduction as applied to the edges of the subtree in the
previous section's example.

~~~~
# subtree_reduction
4	5	32
5	1	62
5	7	39
7	8	58
7	10	53
7	11	39
10	3	66
10	9	53
~~~~

### An example selection file 

The following selection file was generated by selecting the base component (the
component representing arc 4 → 5) of the tree file located at
`examples/tutorial/tree.tree`. Additionally, the color map file located at
`examples/tutorial/tree.colors` was used. Note that the file is
**tab-delimited**.

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
# reduction
32
# subtree
9
1	62
3	66
4	16
5	32
7	39
8	58
9	51
10	53
11	30
4	5	0	25
5	1	2	45
5	7
7	8
7	10
7	11
10	3	6	64
10	9
# subtree_reduction
4	5	32
5	1	62
5	7	39
7	8	58
7	10	53
7	11	39
10	3	66
10	9	53
~~~~
