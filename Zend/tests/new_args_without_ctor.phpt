--TEST--
Argument of new on class without constructor are evaluated
--FILE--
<?php

function fn1697930998()
{
    new stdClass(print 'a', print 'b');
}
fn1697930998();
--EXPECT--
ab
