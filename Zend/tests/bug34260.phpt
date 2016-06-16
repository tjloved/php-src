--TEST--
Bug #34260 (Segfault with callbacks (array_map) + overloading)
--FILE--
<?php

class Faulty
{
    function __call($Method, $Args)
    {
        switch ($Method) {
            case 'seg':
                echo "I hate me\n";
                break;
        }
    }
    function NormalMethod($Args)
    {
        echo "I heart me\n";
    }
}
function fn1982896874()
{
    $Faulty = new Faulty();
    $Array = array('Some junk', 'Some other junk');
    // This causes a seg fault.
    $Failure = array_map(array($Faulty, 'seg'), $Array);
    // This does not.
    $Failure = array_map(array($Faulty, 'NormalMethod'), $Array);
}
fn1982896874();
--EXPECT--
I hate me
I hate me
I heart me
I heart me
