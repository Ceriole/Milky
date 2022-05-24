Contributing to Milky
======================

Thanks for your interest in contributing to Milky! :green_heart:
[Pull requests](https://www.quora.com/GitHub-What-is-a-pull-request) for bugfixes and contributions are very welcome.

These guidelines are intended to help smooth that process, and allow me to review and approve your changes quickly and easily. Improvements are always welcome! Feel free to [open an issue][issue-tracker] or [submit a new pull request][submit-pr].

## Reporting Bugs

Bugs should be reported on the [GitHub Issue Tracker][issue-tracker] using the bug report template (TODO).

Follow the advice in [How do I ask a good question?][how-to-ask]. While the article is intended for people asking questions on Stack Overflow, it all applies to writing a good bug report too.

## Requesting New Features

Feature requests should also be sent to the [GitHub Issue Tracker][issue-tracker] using the feature request template (TODO).

- Explain the problem that you're having, and anything you've tried to solve it using the currently available features.

- Explain how this new feature will help.

- If possible, provide an example, like a code snippet, showing what your new feature might look like in use.

Much of the advice in [How do I ask a good question?][how-to-ask] applies here too.

## Contributing a Fix or Feature

1. Fork the Milky repository.

2. Create a topical branch, and make your changes on that branch.

3. Submit a pull request detailing your changes.

4. Give me a moment! I'm only one bug, this is currently just a passion project for me.

If you're not sure what any of that means, check out Thinkful's [GitHub Pull Request Tutorial][thinkful-pr-tutorial] for a complete walkthrough of the process.

### Writing a Good Pull Request

- Stay focused! Limit yourself to a single fix or feature. Adding multiple features in one pull request makes it harder to control what kind of code goes into the codebase.

- Limit your changes to only what is required to implement the fix or feature. Avoid styling other parts of the code you do not want to modify (i.e. Formatting tools).

- Use descriptive commit titles/messages. "Implemented \<feature\>" or "Fixed \<problem\> is better than "Updated \<file\>".

- Make sure the code you submit compiles and runs without issues.

- Follow the coding conventions, which are quite small.

### Coding Conventions

- Naming convention:
  - For functions we use pascal case: **`FunctionName`**.
  - For (scoped) variables and function parameters we use camel case: **`variableName`** and **`parameterName`**.

  - For class names we use pascal case: **`ClassName`**.

  - For class variables we use the Hungarian notation:
    - Class member variables get the 'm_' prefix: **`m_ClassMemberVariableName`**.
    - Class static variables get the 's_' prefix: **`s_ClassStaticVariableName`**.

  - For macros we use snake case: **`MACRO_NAME`**.
    - If it is specifically related to Milky, we add the 'ML_' prefix: **`ML_MACRO_NAME`**.
    - If there is a macro for the application and for the engine, we add an additional 'CORE_' prefix to the engine macro:  **`ML_CORE_MACRO_NAME`**.

- Use tabs for indentation, not spaces.

- When in doubt, match the code that's already there.

**Thank you for reading!**



[github]: https://github.com
[how-to-ask]: https://stackoverflow.com/help/how-to-ask
[issue-tracker]: https://github.com/Ceriole/Milky/issues
[submit-pr]: https://github.com/Ceriole/Milky/pulls
[thinkful-pr-tutorial]: https://www.thinkful.com/learn/github-pull-request-tutorial/
[github-help-closing-keywords]: https://help.github.com/en/articles/closing-issues-using-keywords
