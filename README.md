GammaSearch
=======

Initial Commit
------------

This initial commit is an old code, along with relevant input files, to search for gammas of interest. The main code, GammaSearch.cxx, has the old changes in place. The next sections will explain the end goal of this data modification, the list of changes that need to be made, and how they are to be made

### Goal of the modification
1. Identify *known* or *unknown* transitions within a certain energy range (centroid +/- some sigma)
2. With that transition information, find transitions that feed into the start level
3. With that transition information, find transitions that it feeds into.
4. Get cascades to ground state that might be seen as a result.

### Plans to accomplish the goals
1. This is the gamma search program as it already exists
2. While it needs refinement, this is also already partially written in the gamma search function, using spin-parity flags
3. Same as 2, but this is also built into the RIPL-3 file
4. This means continuing 3 until the end energy is 0.0, and enumerating it in a manner that makes sense.

### How to accomplish the goals, from a coding perspective
4 is clearly the most in depth expedition out of these. Below are some thoughts on accomplishing this.
* A recursive function that works until reaching 0.0 makes sense. Would it be the most efficient?
* It only needs to search up to the current state, and not farther.
* Print out the flagged chain as it is found, go back up one level, and repeat until all transitions in that level are done, then go up again.

Here is an idea for how the this would look printed out:
```
Transition A  
Cascade 1  
Energy		Intensity	Conv. Coef.	Start Energy	Start Jp	 End Energy	End Jp  
Transition B  
Transition C1
Transition D1

Cascade 2  
Energy		Intensity	Conv. Coef.	Start Energy	Start Jp	 End Energy	End Jp  
Transition B  
Transition C1
Transition D2
Transition E

Cascade 3
Energy		Intensity	Conv. Coef.	Start Energy	Start Jp	 End Energy	End Jp  
Transition B  
Transition C2
...

etc.
```