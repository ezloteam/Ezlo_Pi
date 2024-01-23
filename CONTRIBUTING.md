# Contributing


Thank you for considering contributing to EzloPi! We value your interest and support.

Contributions to EzloPi can take various forms, such as testing newly released features, providing pull requests for new features, proof of concepts, language files, or fixes, and contributing missing documentation for features and devices in our documentation.

This document outlines guidelines for interacting with the issue tracker and pull requests within this repository to ensure a streamlined and effective collaboration process.

## Opening New Issues

1. Opening an issue signifies the existence of a problem in the code that requires attention from the project contributors.
2. When creating an issue, it is mandatory to complete the provided template. The requested information is crucial! Ignoring the template or providing insufficient information may result in the closure of the issue.
4. Issues concerning topics already covered in the documentation will be similarly closed.
5. Issues related to unmerged PRs will be closed. Any issues with a PR should be documented within the PR itself.
6. Issues accompanied by investigations revealing the root cause of the problem will be prioritized.
7. Duplicate issues will be closed.

## Triaging of Issues/PR's

1. Any project contributor can participate in the triaging process if they choose to do so.
2. Issues that need closure, either due to non-compliance with this policy or other reasons, should be closed by a contributor.
3. Accepted issues should be labeled appropriately.
4. Issues with the potential to impact functionality for many users should be considered severe.
5. Issues resulting from the SDK or chip should not be labeled severe, as there might be limited solutions. Use common sense when deciding, and document such issues in the Wiki for user reference.
6. Feature requests' viability and desirability should be discussed.
7. Feature requests or changes addressing specific/limited use cases, especially at the expense of increased code complexity, may be denied or may require redesign, generalization, or simplification.
8. Feature requests not accompanied by a PR:
   - May be closed immediately (denied).
   - May be closed after a predetermined period (left as a candidate for someone to pick up).

## Pull Requests

A Pull Request (PR) is the process through which code modifications are managed on GitHub.

The process is straightforward.

- Fork the EzloPi Repository git repository.
- Write/Change the code in your Fork for a new feature, bug fix, new sensor, optimization, etc.
- Header, Source and CMakeList files should accurately follow [this](ezlopi-src-template/Readme.md) guideline.
- Ensure tests pass.
- Create a Pull Request against the **development** branch of EzloPi.

1. All pull requests must target the development branch.
2. Modify only relevant files (Also, be cautious if your editor...[additional content not provided in the original reference]).

--------------------------------------

## Contributor License Agreement (CLA)

```
By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the EASL license; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the EASL license; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it) is maintained indefinitely
    and may be redistributed consistent with this project or the open
    source license(s) involved.
```

We appreciate your contributions and adherence to these guidelines, making the EzloPi project better for everyone.