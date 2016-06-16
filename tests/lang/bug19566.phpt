--TEST--
Bug #19566 (get_declared_classes() segfaults)
--FILE--
<?php

class foo
{
}
function fn1132021543()
{
    $result = get_declared_classes();
    var_dump(array_search('foo', $result));
}
fn1132021543();
--EXPECTF--
int(%d)
