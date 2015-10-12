use r1emu;


INSERT INTO `accounts` (`account_id`, `account_name`, `passwd`, `is_banned`, `time_banned`, `credits`, `privilege_level`, `time_last_login`, `family_name`, `barrack_type`) VALUES
(1, 'r1emu', '43467e0995aa61536c85276e14146913', 'n', '0000-00-00 00:00:00', 123, 0, '0000-00-00 00:00:00', 'r1emu', 0), # login/password : r1emu/r1emu
(2, 'r2emu', 'e33eb5e2effe4f6678739f9a90ed6f6c', 'n', '0000-00-00 00:00:00', 123, 3, '0000-00-00 00:00:00', 'r1emu', 0); # login/password : r2emu/r2emu

INSERT INTO `commanders` (`commander_id`, `commander_name`, `account_id`, `last_login`, `last_logout`, `time_created`, `is_deleted`, `time_deleted`, `level`, `exp`, `gender`, `job_id`, `class_id`, `hair_id`, `map_id`, `position_x`, `position_y`, `position_z`, `barrack_position_x`, `barrack_position_y`, `barrack_position_z`, `hp`, `mp`, `eqslot_head_top`, `eqslot_head_middle`, `eqslot_unkown_1`, `eqslot_body_armor`, `eqslot_gloves`, `eqslot_boots`, `eqslot_helmet`, `eqslot_bracelet`, `eqslot_weapon`, `eqslot_shield`, `eqslot_costume`, `eqslot_unkown_3`, `eqslot_unkown_4`, `eqslot_unkown_5`, `eqslot_leg_armor`, `eqslot_unkown_6`, `eqslot_unkown_7`, `eqslot_ring_left`, `eqslot_ring_right`, `eqslot_necklace`) VALUES
(1,  'HelloMoto', 2, '0000-00-00 00:00:00', '0000-00-00 00:00:00', '0000-00-00 00:00:00', 'n', '0000-00-00 00:00:00', 10, 123, 2, 3006, 10003, 6,  1021, -623, 260, -1044, 10, 20, 30, 238, 105, 2, 2, 4, 531101, 6, 7, 10000, 11000, 161101,  9999996, 4, 9, 9, 4, 521101, 9, 9, 9, 9, 10),
(2,  'Gaston',    1, '0000-00-00 00:00:00', '0000-00-00 00:00:00', '0000-00-00 00:00:00', 'n', '0000-00-00 00:00:00', 10, 123, 2, 3006, 10003, 6,  1021, -623, 260, -1044, 10, 20, 30, 238, 105, 2, 2, 4, 531101, 6, 7, 10000, 11000, 161101,  9999996, 4, 9, 9, 4, 521101, 9, 9, 9, 9, 10),
(33, 'bbbb',      2, '0000-00-00 00:00:00', '0000-00-00 00:00:00', '0000-00-00 00:00:00', 'n', '0000-00-00 00:00:00', 1,  12,  1, 2001, 10006, 11, 1021, -623, 260, -1044, 19, 28, 29, 238, 105, 2, 2, 4, 8,      6, 7, 10000, 11000, 9999996, 9999996, 4, 9, 9, 4, 8,      9, 9, 9, 9, 10),
(16, 'Tester',    1, '0000-00-00 00:00:00', '0000-00-00 00:00:00', '0000-00-00 00:00:00', 'n', '0000-00-00 00:00:00', 1,  12,  1, 2001, 10006, 11, 1021, -623, 260, -1044, 19, 28, 29, 238, 105, 2, 2, 4, 531101, 6, 7, 10000, 11000, 201101,  9999996, 4, 9, 9, 4, 521101, 9, 9, 9, 9, 10);

