--TEST--
Trying declare interface with repeated name of inherited method
--FILE--
<?php

interface a
{
    function b();
}
interface b
{
    function b();
}
interface c extends a, b
{
}
function fn2043260179()
{
    echo "done!\n";
}
fn2043260179();
--EXPECTF--
done!
