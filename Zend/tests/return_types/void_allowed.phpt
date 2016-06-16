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
function fn126959184()
{
    foo();
    bar();
    echo "OK!", PHP_EOL;
}
fn126959184();
--EXPECT--
OK!
