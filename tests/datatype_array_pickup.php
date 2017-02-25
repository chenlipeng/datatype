<?php
$arr = array(
    'a' => array(
        'aa'    => 'dddd',
        'bb'    => 'bb',
        'cc'    => 'cc',
        'dd'    => 'dd',
        'ee'    => 'ee',
    ),
    'b' => array(
        'aa'    => 'aaaa',
        'bb'    => 'bbbb',
        'cc'    => 'cc',
        'dd'    => 'dddd',
        'ee'    => 'eeee',
    ),
    'c' => array(
        'aa'    => 'aaaaaa',
        'bb'    => 'bbbbbb',
        'cc'    => 'cccccc',
        'dd'    => 'dddddd',
        'ee'    => 'eeeeee',
    ),
    'd' => array(
        'aa'    => 'aaaaaaaa',
        'bb'    => 'bbbbbbbb',
        'cc'    => 'cccccccc',
        'dd'    => 'dddddddd',
        'ee'    => 'eeeeeeee',
    ),
    'e' => array(
        'aa'    => 'aaaaaaaaaaa',
        'bb'    => 'bbbbbbbbbbb',
        'cc'    => 'ccccccccccc',
        'dd'    => 'ddddddddddd',
        'ee'    => 'eeeeeeeeeee',
    ),
    'f' => array(
        'aa'    => 'aaaaaaaaaaa',
        'bb'    => 'bbbbbbbbbbb',
        'cc'    => 'ccccccccccc',
        'dd'    => 'ddddddddddd',
        'ee'    => 'eeeeeeeeeee',
    ),
);

$index = 0;
do {
    //$ret = array_column($arr, array('aa', 'bb', 'cc'), 'ee');
    $ret = datatype_array_pickup($arr, array('aa', 'bb', 'cc', 'dd', 'ee', 'ff', 'gg'), 'cc', PICKUP_VD_MERGE);
    //$ret = datatype_array_pickup($arr, array('aa', 'bb', 'cc', 'dd', 'ee', 'ff', 'gg'), 'cc', PICKUP_VD_MERGE);
    var_dump($ret);
} while ($index-- > 0);
var_dump($index);
