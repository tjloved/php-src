--TEST--
Test to check regressions on T_IMPLEMENTS followed by a T_NS_SEPARATOR
--FILE--
<?php

interface A
{
}
class B implements \A
{
}
function fn2037059589()
{
    echo "Done", PHP_EOL;
}
fn2037059589();
--EXPECTF--

Done
