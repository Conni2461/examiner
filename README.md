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
- [ ] Documentation
  - [ ] Guide in github
  - [ ] `--help`
  - [ ] man page
  - [ ] header documentation
- [ ] enable/disable color. on / off / auto
- [ ] `--list-tests`
- [ ] `--short`
- [ ] `--shuffle` test order
- [ ] `--repeat` run tests multiple times
- [ ] `--die-on-failure` stop executing on fail
- [ ] make install
