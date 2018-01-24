# Contributing

You can contribute by:

- writing code
- researching the original engine
- testing
- discussing with the community

## Writing Code

Ideal code contribution looks like this:

1. Pick a task - currently either from the GitHub issue tracker or your imagination (larger tasks are always better to be discussed first, get in touch on discord server mentioned in README.md).
2. Fork the repository if you haven't already.
3. Make a branch for the task you're implementing (e.g. `git branch my-feature`).
4. Implement the task
   - Stick to our **code formatting style** (the same as in [OpenMW](https://wiki.openmw.org/index.php?title=Code_Formatting_Conventions)) and good programming practices (descriptive identifiers etc.).
   - Use Git best practices (commit often, write [descriptive and well-formatted](https://chris.beams.io/posts/git-commit/) commit messages).
5. Make a [pull request](https://help.github.com/articles/about-pull-requests/).
   - You can (and are encouraged to) make a PR even if you're not finished yet so that we can see what you're working on, can give you feedback and discuss your code. In such case, label the title with `Don't merge!`.
6. Wait for discussion and reviews, address any issues before the merge. You may want to [squash](https://git-scm.com/book/en/v2/Git-Branching-Rebasing) some smaller commits together if there is too many of them etc.

**When in doubt, take a look at similar projects' approach** ([OpenMW](https://github.com/OpenMW/openmw), [OpenRW](https://github.com/rwengine/openrw) etc.).

We sometimes break these rules in the current early stage of the project, but with more code and devs they'll become more important and also more strictly enforced.

## Researching the Original Engine

Large parts of the game (formats, game formulas, mechanics, ...) are uncovered and have yet to be reverse-engineer/researched and documented. To find out what's to be researched (and how) ask us on discord (see README.md for the link) or take a look at our GitHub issue tracker. Currently documentation can be contributed in following ways:

- Directly in the code, e.g. make a pull request with a parser of an unknown format.
- Writing a description and commiting to /docs folder. (Note that not all currently uncovered formats are in the folder, we didn't get to write the docs yet.)
- Writing or finding a description and sending it to us, preferably via our discord (see README.md for the link).

Note that the original game runs well even on Linux under Wine.

## Testing

Simply play around with the engine and report any yet unreported bugs to our GitHub issue tracker - make a new issue per bug - or tell us via our discord (see README.md for the link).

For effective testing:
- use tool such as valgrind, cppcheck etc.
- find a way to reproduce given bug
- provide info on your testing environment (software versions, OS, GPU, compiler etc.)

We'll also need automated tests later, so if you're interested in making some, we'll be more than glad to hear from you.

## Discussing With the Community

Come talk to us on discord (see README.md for the link) about anything, make suggestions, ask questions etc.
