--TEST--
Bug #19566 (get_declared_classes() segfaults)
--FILE--
<?php

class foo
{
}
function fn982736490()
{
    $result = get_declared_classes();
    var_dump(array_search('foo', $result));
}
fn982736490();
--EXPECTF--
int(%d)
