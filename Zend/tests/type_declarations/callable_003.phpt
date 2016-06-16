--TEST--
callable type#003
--FILE--
<?php

function foo(callable $a, $b, callable $c)
{
    var_dump($a, $b, $c);
}
function bar(callable $a = null)
{
    var_dump($a);
}
function fn2062413672()
{
    foo("strpos", 123, "strpos");
    bar("substr");
}
fn2062413672();
--EXPECT--
string(6) "strpos"
int(123)
string(6) "strpos"
string(6) "substr"

