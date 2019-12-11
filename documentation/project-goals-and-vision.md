# Project Goals and Vision
This document is provided as a means of outlining the goals, requirements and vision of the project. There are a number of sources for this information, including the Project Owner, The Kestrel Engine Project and the Community.

This is not an absolutely concrete document, though certain requirements from Kestrel may be concrete from the perspective of the KDK.


## Project Vision
The idea of this development kit and set of tools is to make plugin and content development easy for those wishing to make content for the Kestrel Game Engine. The project should be written and developed to a modern standard, be portable to any platform where Kestrel itself may run, and be easily interfaced with by those wishing to create graphical development tools.

The way I (as the Project Owner) envision KDK evolving is towards a tool that has the following functions and capabilities:

- Ability to take text based resource definitions and assemble them into binary resources, readable by either EV Nova and/or Kestrel.
- Ability to take image assets (PNG and TGA) and include them into resource files as (PïNG or TäRG) resources when targeting Kestrel. If the plugin is targeting EV Nova then it should convert these resources into the appropriate format (either cicn, rlëD or PICT). _The EV Nova target exists as a requirement for the development of the EV Override Remastering_
- The ability to take existing resource files and disassemble them into KDL files and image assets.
- The ability to link against a game Scenario and use that as a basis for validating the content of the resources being assembled.
- The ability to have all elements of a plugin/scenario that is being developed be checked into a version control system in a useful and meaningful way.
- To be ultimately community led, and evolve to suit the needs of the community.

## Community Led
This is something that I have mentioned a lot, and I want to clarify what I mean by this. Obviously if it is just a free for all of community commits this project will go nowhere fast.

Initially it will have heavy guidance from Project Owners as it is brought upto a usable state. This is because the project needs to evolve and move quickly due to requirements from the _EV Override Remastering Project_ which will use the Kestrel engine.

Eventually the only real guidance and requirements will be coming from the Kestrel engine. Obviously if Kestrel introduces, removes or changes something then KDK will need to respond to that change. Beyond that it will the decision of the community on how to evolve the project. Changes to KDL, supporting other definition languages, the features of the assembler, integrating with the engine, etc.

Ultimately there will be Maintainers appointed for the project who will run the project on a day to day basis.