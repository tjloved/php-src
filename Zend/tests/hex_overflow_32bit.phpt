--TEST--
testing integer overflow (32bit)
--INI--
precision=14
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

function fn1064738832()
{
    $doubles = array(0x1736123fffaaa, 4.722366482869645E+21, 0.0, 1.9807040628566085E+27);
    foreach ($doubles as $d) {
        $l = $d;
        var_dump($l);
    }
    echo "Done\n";
}
fn1064738832();
--EXPECTF--	
float(4.0833602971%dE+14)
float(4.7223664828%dE+21)
float(1.3521606402%dE+31)
float(1.9807040628%dE+27)
Done
