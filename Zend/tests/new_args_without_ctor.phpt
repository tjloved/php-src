--TEST--
Argument of new on class without constructor are evaluated
--FILE--
<?php

function fn1057723975()
{
    new stdClass(print 'a', print 'b');
}
fn1057723975();
--EXPECT--
ab
