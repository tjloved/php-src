--TEST--
Testing creation of alias to an internal class
--FILE--
<?php

function fn842003245()
{
    class_alias('stdclass', 'foo');
}
fn842003245();
--EXPECTF--
Warning: First argument of class_alias() must be a name of user defined class in %s on line %d
