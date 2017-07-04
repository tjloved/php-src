--TEST--
Creating instances dynamically
--FILE--
<?php

function fn968639623()
{
    $arr = array(new stdClass(), 'stdClass');
    new $arr[0]();
    new $arr[1]();
    print "ok\n";
}
fn968639623();
--EXPECT--
ok
