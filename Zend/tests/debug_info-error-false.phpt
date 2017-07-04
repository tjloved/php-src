--TEST--
Testing __debugInfo() magic method with bad returns FALSE
--FILE--
<?php

class C
{
    public $val;
    public function __debugInfo()
    {
        return $this->val;
    }
    public function __construct($val)
    {
        $this->val = $val;
    }
}
function fn2344072()
{
    $c = new C(false);
    var_dump($c);
}
fn2344072();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-false.php on line %d
