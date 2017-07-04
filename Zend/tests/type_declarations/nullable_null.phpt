--TEST--
nullable class
--FILE--
<?php

function test(Foo $a = null)
{
    echo "ok\n";
}
function fn445726169()
{
    test(null);
}
fn445726169();
--EXPECT--
ok
