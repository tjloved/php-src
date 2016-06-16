--TEST--
Creating instances dynamically
--FILE--
<?php

function fn2078706707()
{
    $arr = array(new stdClass(), 'stdClass');
    new $arr[0]();
    new $arr[1]();
    print "ok\n";
}
fn2078706707();
--EXPECT--
ok
