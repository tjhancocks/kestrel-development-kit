# Kestrel Development Kit
The Kestrel Development Kit aims to be a set of tools to aide and make it possible to
develop content for the [Kestrel Game Engine](https://github.com/tjhancocks/kestrel.git).

## About
The Kestrel Development Kit is currently comprised of two major components. These are

- Kestrel Definition Language (KDL)
- Kestrel Assembler (kas)

Both of these are intended to allow content developers to produce content for the Kestrel engine, by converting definition files and image assets into a binary resource file format that is understood by Kestrel.

Both _kas_ and _KDL_ should allow content developers to be detached from the evolution of the Kestrel engine, and not having to consider specific changes to the resource format of Kestrel.

### Kestrel Assembler
The Kestrel Assembler is a command line tool that takes an input file and 'assembles' it into a Kestrel Resource File. At present resource definitions are supplied in the form of a KDL file.

An example of invoking the assembler might be

```zsh
kas -o plugin.kdat -f plugin.kdl
```

### Kestrel Definition Language
The Kestrel Definition Language is a specially designed _Domain Specific Language_ (DSL) for representing Kestrel Resources in a textual format. See the [KDL specification](documentation/kdl-specification.md) for complete documentation.

Below is an example of the _KDL_

```kdl
` Add a new NPC character in to the game.

declare NPC {
	new (id = #500, name = "snijj") {
		spawn_system = government #128;
		affiliation = #128;
		ai_type = brave_trader;
		aggression_range = close;
		cowardice = 15%;
		ship = #129;
		weapon = #140 1 100;
		weapon = #141 1 400;
		credits = 1000;
		hail = file("snijj_hail_pic.png");
		comm_quote = 5;
		hail_quote = 7;
		flags = grudge | hail_grudge;
	}
}

```
