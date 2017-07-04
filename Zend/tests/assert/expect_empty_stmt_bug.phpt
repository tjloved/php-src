--TEST--
Empty statement in assert() shouldn't segfault
--FILE--
<?php

function fn715441265()
{
    assert((function () {
        return true;
    })());
    echo "ok";
}
fn715441265();
--EXPECT--
ok
