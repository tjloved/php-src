--TEST--
Using the same function name on interface with inheritance
--FILE--
<?php

interface Itest
{
    function a();
}
interface Itest2
{
    function a();
}
interface Itest3 extends Itest, Itest2
{
}
function fn113538398()
{
    echo "done!\n";
}
fn113538398();
--EXPECTF--
done!
