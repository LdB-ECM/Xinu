Teaching with Embedded Xinu
===========================

.. toctree::
   :glob:
   :hidden:

   *
   assignments/index

The pages linked to here help an instructor understand how to
incorporate Embedded Xinu into a course for his or her department's
curriculum.  Each of the models is based on an existing course that
has been run at Marquette or one of our partner schools.

Operating Systems Tracks
------------------------

Student Built Xinu
^^^^^^^^^^^^^^^^^^

*Main article:* :doc:`Student-Built-Xinu`

A student built operating system puts the student in the trenches of
operating system development. The student will become intimately
involved with the inner workings of an operating system. This will give
the student a better understanding of the various systems that work
together behind the scenes while an operating system is running.
Operating systems topics that can be incorporated in a student built
Xinu course include: memory management, scheduling, concurrent
processing, device management, file systems and others.

Student Extended Xinu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Main article:* :doc:`Student-Extended-Xinu`

Students will learn to extend an operating system by adding kernel level
and user level applications. Given a functional Embedded Xinu operating
system the students will have to understand and manipulate existing
operating system code to create additional operating system features. To
add more applications to the operating system students will have to
understand the interactions between the program in design and the
operating system's device and kernel interaction calls. Programming for
embedded devices allows students to engage in development on small
resource constrained environments. Through extending the existing
Embedded Xinu operating system a student learns to use and understand
code not written by the student and develops advanced operating system
concepts.

Networking
----------

Networking With Xinu
^^^^^^^^^^^^^^^^^^^^

*Main article:* :doc:`Networking-With-Xinu`

A networking course incorporating Embedded Xinu can have students build
networking functionality into Embedded Xinu over the period of the
course. Courses may vary in starting point; some choosing to use a core
Embedded Xinu release and having students build the entire network stack
and ethernet driver. While others may choose an Embedded Xinu release
with the ethernet driver available so that the students can concentrate
on other parts of the network stack. Network stack implementation
assignments for students can parallel various networking lectures that
traverse the stack over the course of the semester, terminating in the
students implementing an application that uses the developed network
stack.

Compilers
---------

Compiler Construction With Embedded Xinu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Main article:* :doc:`Compiler-Construction-With-Embedded-Xinu`

Including Embedded Xinu in a compiler construction course allows
students to explore the compilation of high level language constructs
that rely on interacting with the underlying runtime. Many traditional
compilers courses simply target a processor or simulator, but by
targeting a *platform* (a processor and operating system combination)
one can extend the source language to include more advanced language
features such as I/O operations and thread creation, manipulation, and
concurrency. This also allows students to run their test cases on real
hardware and see these programs actually interacting with a real
runtime. In modern programming these high level language features are
vital, and it is important for students to see what the processor and
runtime are doing when they use these features in their own programs.

Building a Backend Pool
-----------------------

Regardless of the exact instructional model chosen, the instructor may
wish to set up a classroom or laboratory setting enabling easy
modification and use of Embedded Xinu.  More information can be found
in :doc:`Laboratory`.

Workshops
---------

Workshops have been held regarding teaching with Embedded Xinu. For
example, the `Teaching With Embedded Xinu Workshop
<http://www.cs.olemiss.edu/acmse2010/pdf/xinu.pdf>`__ at `ACMSE 2010
<http://www.cs.olemiss.edu/acmse2010/Home.htm>`__ in Oxford,
Mississippi (Ole Miss campus) shared ready-made curriculum resources
that have been used successfully to teach hardware systems, operating
systems, realtime/embedded systems, networking, and compilers with the
Embedded Xinu platform at several colleges/universities.

Acknowledgements
----------------

This work funded in part by NSF grant DUE-CCLI-0737476.
