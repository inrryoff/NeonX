# NeonX Development Workflow Mandates

## Staging & Validation
- **Isolation:** For every new code modification task, a dedicated temporary working directory must be created.
- **Encapsulation:** All source code and build files must be copied into the staging directory before any edits are performed.
- **Preservation Principle:** NEVER modify existing, functional code unless:
    1. Explicitly requested by the user.
    2. Strictly necessary to fix a verified bug or vulnerability.
    3. Mandatory for the implementation or removal of a specific feature.
    - Avoid "cleanup", "refactoring", or "styling" of unrelated code parts.
- **Independent Build:** A custom `Makefile` must be created and maintained within the staging directory to validate changes independently.
- **Final Delivery (`ok_source`):** Once a task is fully validated and stable, the final source code and its corresponding `Makefile` must be moved/copied to the `ok_source/` directory.
- **Persistence:** This workflow is a foundational mandate for all future turns in this project.
