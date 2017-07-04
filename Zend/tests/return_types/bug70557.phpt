--TEST--
Bug #70557 (Memleak on return type verifying failed).
--INI--
opcache.enable=0
--FILE--
<?php

function getNumber() : int
{
    return "foo";
}
function fn374582349()
{
    try {
        getNumber();
    } catch (TypeError $e) {
        var_dump($e->getMessage());
    }
}
fn374582349();
--EXPECT--
string(72) "Return value of getNumber() must be of the type integer, string returned"
