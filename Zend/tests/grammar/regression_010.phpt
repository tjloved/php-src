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
function fn1551508354()
{
    echo "Done", PHP_EOL;
}
fn1551508354();
--EXPECTF--

Done
