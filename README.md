# examiner

A small, opinionated c unit testing framework.

## Documentation

For now take a look at `test/examiner_test.c` but note this is not yet done.

## TODO:

- [x] More test definitions `PENDING`
- [x] Assert more than just int
- [x] When filtering it will still say the full filter count at the beginning
- [x] Auto register TEST, no idea how. Good Luck to myself
- [ ] Easy multifile support
- [x] `--filter` needs substr matching so i can only run one namespace
- [x] timer how loong the execution of a test took
- [ ] separator between namespaces (either a `\n` or `string.rep(78, '-')`
  - [ ] better grouping even if they are not sorted in the file.
        (scope grouping)
- [ ] Documentation
  - [ ] Guide in github
  - [x] `--help`
  - [ ] man page
- [x] enable/disable color. on / off
- [x] `--list-tests`
- [ ] `--short`
- [ ] `--shuffle` test order
- [x] `--repeat` run tests multiple times
- [x] `--die-on-failure` stop executing on fail
- [ ] make install
