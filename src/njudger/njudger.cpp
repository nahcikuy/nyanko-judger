#include "initializer.hpp"
#include "preRunner.hpp"
#include "runner.hpp"

using namespace njudger;

int main(int argc, char **argv)
{
    initialize(argc, argv), checkSrcFileSize(), compile(), run();
}
