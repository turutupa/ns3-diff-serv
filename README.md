# Differentiated Services
Authors
- Alberto Delgado
- Jason McGowan

## Requirements
1. Download ns-3: https://www.nsnam.org/releases/ns-3-38/download/
2. Install JSON parser: https://github.com/nlohmann/json

## How to build
- Make sure you have first downloaded ns-3 and installed the JSON parser
- Extract the source files to the ns-3 folder: `/ns-allinone-3.38/ns-3.38/contrib/diff-serv`
- Then inside `/ns-allinone-3.38/` run
```bash
$ ./ns3 configure --enable-tests --enable-examples
$ ./ns3 build # this may take a while
```

## How to clean build files
- To clean, make sure to be in `/ns-allinone-3.38/` then:
```bash
$ ./ns3 clean
```

## How to run
- To run tests, make sure to be in `/ns-allinone-3.38/` then:
```bash
$ ./test.py -s diff-serv
$ ./test.py -s diff-serv -v # with optional verbose flag
```
- To run the examples, make sure to be in `/ns-allinone-3.38/` then:
```bash
$ ./test.py -e diff-serv-example
$ ./test.py -e diff-serv-example -v # with optional verbose flag
```
