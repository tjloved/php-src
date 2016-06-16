--TEST--
Bug #34358 (Fatal error: Cannot re-assign $this(again))
--FILE--
<?php

class foo
{
    function bar()
    {
        $ref =& $this;
    }
}
function fn479850462()
{
    $x = new foo();
    $x->bar();
    echo "ok\n";
}
fn479850462();
--EXPECT--
ok
