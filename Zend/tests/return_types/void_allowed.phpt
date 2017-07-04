--TEST--
void return type: acceptable cases
--FILE--
<?php

function foo() : void
{
    // okay
}
function bar() : void
{
    return;
    // okay
}
function fn1951803797()
{
    foo();
    bar();
    echo "OK!", PHP_EOL;
}
fn1951803797();
--EXPECT--
OK!
