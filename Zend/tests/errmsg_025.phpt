--TEST--
errmsg: cannot inherit previously inherited constant
--FILE--
<?php

interface test1
{
    const FOO = 10;
}
interface test2
{
    const FOO = 10;
}
class test implements test1, test2
{
}
function fn916511713()
{
    echo "Done\n";
}
fn916511713();
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant FOO from interface test2 in %s on line %d
