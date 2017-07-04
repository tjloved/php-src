--TEST--
ZE2 iterators must be implemented
--FILE--
<?php

class c1
{
}
class c2
{
    public $max = 3;
    public $num = 0;
    function current()
    {
        echo __METHOD__ . "\n";
        return $this->num;
    }
    function next()
    {
        echo __METHOD__ . "\n";
        $this->num++;
    }
    function valid()
    {
        echo __METHOD__ . "\n";
        return $this->num < $this->max;
    }
    function key()
    {
        echo __METHOD__ . "\n";
        switch ($this->num) {
            case 0:
                return "1st";
            case 1:
                return "2nd";
            case 2:
                return "3rd";
            default:
                return "???";
        }
    }
}
function fn1402425913()
{
    echo "1st try\n";
    $obj = new c1();
    foreach ($obj as $w) {
        echo "object:{$w}\n";
    }
    echo "2nd try\n";
    $obj = new c2();
    foreach ($obj as $v => $w) {
        echo "object:{$v}=>{$w}\n";
    }
    print "Done\n";
}
fn1402425913();
--EXPECTF--
1st try
2nd try
object:max=>3
object:num=>0
Done
