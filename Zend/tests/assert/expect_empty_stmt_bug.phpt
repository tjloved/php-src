--TEST--
Empty statement in assert() shouldn't segfault
--FILE--
<?php

function fn371648464()
{
    assert((function () {
        return true;
    })());
    echo "ok";
}
fn371648464();
--EXPECT--
ok
