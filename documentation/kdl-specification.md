# KDL Specification

| Item | Information |
| --- | --- |
| Authors | Tom Hancocks |
| Revision | 1 |
| Last Edit Date | December 4th, 2019 |

## About
The _Kestrel Definition Language_, or _KDL_, is a Domain Specific Language created and designed to allow content developers a convenient way of representing Kestrel resources in a plain text format.

This document is intended to outline and specify the structure, concepts and philosophies behind KDL. Furthermore, this document should not be considered final, and proposals to change, ammend or extend this specification will be reviewed.

## Philosophy
KDL is intended to be an expressive, but basic language with the sole purpose of representing resources for use in Kestrel. The language should be expressive enough to making reading and understanding intent straightforward and not require consulting documentation in order to decipher meaning.

An example of what this means is in representing flags. Kestrel uses flags in a number of resources. Flags are simply a bit of information (on or off) that tells the game engine that it should or shouldn't do something. Typically they are grouped together into a single number. A 16-bit number may contain upto 16 individual flags. These are typically represented in the following way:

```
flags = 0x003A
```

Which can expanded out and shown in binary as

```
flags = 0000 0000 0011 1010
```

Here each digit (0 or 1) represents a single bit or flag. If the digit is 1 then the flag is turned on, otherwise it is off.

This representation is compact but it is not friendly to the person reading, or writing the code.

KDL solves this by assigning a name to each flag, such as `food`, `metal`, `medical`, etc. If you provide the name of the flag, then it infers that the flag should be set.

```
cargo = food medical;
```

This way it is easier for the reading and developer to infer the meaning. This same approach is taken throughout KDL.

## Structure
KDL follows a very simple structure. The structure is taken to mean the method by which code is written and how it is laid out. This structure can very roughly be represented as:

- KDL Document (Order of children does not matter)
	- Directive (Multiple)
	- Declaration (Multiple, order of children does not matter)
		- Instance (Multiple attributes, multiple fields)
			- Attribute
			- Field

This fixed structure gives a consistant look and feeling to all KDL documents. Represented as code, this might look something like

```kdl
@out { 
	"Hello, World"
	"The @out directive allows printing messages to the standard output"
}

declare ResourceType {

	` Create a new instance of ResourceType
	new (attributes...) {
		fields...
	}

}

declare AnotherType {

	` Create a new instance of AnotherType
	new (attributes...) {
		fields...
	}

}
```

## Basic Syntax
It is purposefully intended that there are very few syntactic elements to KDL. It is a definition language, not a programming language. It is possible that compiler directives can be added to perform more complex and dynamic tasks. This section will detail some of the basic syntactic elements.

### Comments
KDL does include comments. They are single line and purposefully awkward to use. The intention is for the language to be expressive, and thus shouldn't need to lean on comments heavily. Any shortcomings should be raised as an issue so the issue can be reviewed.

```kdl
` Comments begin with a backtick, and run to the end of the line.
```

### Integers
Integers are represented in a standard decimal format such as

```kdl
42
1337
```

### Resource ID
Resource ID's are seperate entities to integers, and whilst they are technically an integer internally, they are still distinctly represented in KDL.

```kdl
#128
```

### Strings
String literals are represented using the double-quote syntax

```kdl
"This is a string"
```

### Identifier
Identifiers are used to represent the names of entities, flags, types, etc. They have limitations on what characters can be used (`A-Za-z0-9_`).

```kdl
cargo
SpriteAnimation
declare
```

### Directive
Assembler Directives are instructions to the assembler that are executed in place. They always begin with an `@` and typically have an associated body.

```kdl
@import { "missions.kdl" }
```

### Resource Type Declaration
A declaration tells the assembler that the user is about to start defining instances of a given type.

```kdl
declare ResourceType { ... }
```

### Resource Definition
A definition tells the assembler to create a new instance of a given resource type, along with any attributes and fields for it. Definitions are supplied through the `new` keyword.

The structure of a definition looks like

```kdl
new (attributes) { fields }
```

### Resource Attributes
Resource attributes are used to define either the ID or the Name attributes of a resource. They look like either of these.

```kdl
id = #128
name = "Test Resource"
```

The order of attributes does not matter and they are seperated by a comma. Additional attributes may be defined in the future.

- `id` - The value for this can be either a resource id or `auto`. `auto` should instruct the assembler to find the next available resource id.
- `name` - The value for this must be a string.

### Resource Fields
Resource fields assign name value sets to a resource instance, and will be ultimately used by the assembler to construct the actual final binary representation of the resource.

They look like, with each field being allowed to define 1 or more values.

```kdl
field = value1 "value2";
```

Which fields are valid for a given resource type and what values are acceptable is down to the individual resource types and beyond the remit of the KDL specification.