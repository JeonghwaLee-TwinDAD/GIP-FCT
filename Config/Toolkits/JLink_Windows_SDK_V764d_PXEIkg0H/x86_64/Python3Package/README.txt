1) Install the jlinksdk .whl file using pip:

pip install <path/to/.whl>

Note:
When having multiple python interpreter versions installed, different interpreter versions can have different pip versions as well.
This is the case for some Linux distributions, where "pip" refers to pip for Python2 and "pip3" must be used for Python3 packages.
Make sure to install the jlinksdk package using the pip version for the intended python version.

2) Import the jlinksdk inside a python module to use it.
   The jlinksdk_StartupSequence.py script can be used as a template for how to use the jlinksdk package.
3) Please refer to the J-Link SDK documentation for more information on how to use the jlinksdk python package.