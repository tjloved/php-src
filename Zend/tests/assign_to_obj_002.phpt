--TEST--
Assign to $this leaks when $this not defined
--FILE--
<?php

function fn1857297181()
{
    try {
        $this->a = new stdClass();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
fn1857297181();
--EXPECT--
Using $this when not in object context
