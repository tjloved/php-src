--TEST--
The result of a by-ref function call can be yielded just fine
--FILE--
<?php

function &nop(&$var)
{
    return $var;
}
function &gen(&$var)
{
    (yield nop($var));
}
function fn1828812186()
{
    $var = "foo";
    $gen = gen($var);
    foreach ($gen as &$varRef) {
        $varRef = "bar";
    }
    var_dump($var);
}
fn1828812186();
--EXPECT--
string(3) "bar"
