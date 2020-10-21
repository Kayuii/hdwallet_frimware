void fsm_msgInitialize(Initialize *msg) {
    (void)msg;
    recovery_abort(false);
    signing_abort();
    session_clear(false);  // do not clear PIN
    layout_home();
    fsm_msgGetFeatures(0);
}

static const char *model(void) { return "SW100"; }

void fsm_msgGetFeatures(GetFeatures *msg) {
    (void)msg;
    RESP_INIT(Features);

    /* Vendor */
    resp->has_vendor = true;
    strlcpy(resp->vendor, "Digbig", sizeof(resp->vendor));

    /* Version */
    resp->has_major_version = true;
    resp->major_version = MAJOR_VERSION;
    resp->has_minor_version = true;
    resp->minor_version = MINOR_VERSION;
    resp->has_patch_version = true;
    resp->patch_version = PATCH_VERSION;

    /* Device ID */
    resp->has_device_id = true;
    strlcpy(resp->device_id, storage_getUuidStr(), sizeof(resp->device_id));

    /* Model */
    resp->has_model = true;
    strlcpy(resp->model, model(), sizeof(resp->model));

    /* Variant Name */
    resp->has_firmware_variant = false;

    /* Security settings */
    resp->has_pin_protection = true;
    resp->pin_protection = storage_hasPin();
    resp->has_passphrase_protection = true;
    resp->passphrase_protection = storage_getPassphraseProtected();

	//no revision
    resp->has_revision = false;

    /* Bootloader hash */
    resp->has_bootloader_hash = true;
    resp->bootloader_hash.size =
        memory_bootloader_hash(resp->bootloader_hash.bytes, false);

    /* Firmware hash */
    resp->has_firmware_hash = true;
    resp->firmware_hash.size = memory_firmware_hash(resp->firmware_hash.bytes);

    /* Settings for device */
    if (storage_getLanguage()) {
        resp->has_language = true;
        strlcpy(resp->language, storage_getLanguage(), sizeof(resp->language));
    }

    if (storage_getLabel()) {
        resp->has_label = true;
        strlcpy(resp->label, storage_getLabel(), sizeof(resp->label));
    }

    /* Is device initialized? */
    resp->has_initialized = true;
    resp->initialized = storage_isInitialized();

    /* Are private keys imported */
    resp->has_imported = true;
    resp->imported = storage_getImported();

    /* Are private keys known to no-one? */
    resp->has_no_backup = true;
    resp->no_backup = storage_noBackup();

    /* Cached pin and passphrase status */
    resp->has_pin_cached = true;
    resp->pin_cached = session_isPinCached();
    resp->has_passphrase_cached = true;
    resp->passphrase_cached = session_isPassphraseCached();

    /* Policies */
    resp->policies_count = POLICY_COUNT;
    storage_getPolicies(resp->policies);
    _Static_assert(
        sizeof(resp->policies) / sizeof(resp->policies[0]) == POLICY_COUNT,
        "update messages.options to match POLICY_COUNT");

    msg_write(MessageType_MessageType_Features, resp);
}

static void coin_from_token(CoinType *coin, const TokenType *token) {
    memset(coin, 0, sizeof(*coin));

    coin->has_coin_name = true;
    strncpy(&coin->coin_name[0], "ERC20", sizeof(coin->coin_name));

    coin->has_coin_shortcut = true;
    strncpy(&coin->coin_shortcut[0], token->ticker,
            sizeof(coin->coin_shortcut));

    coin->has_forkid = true;
    coin->forkid = token->chain_id;

    coin->has_maxfee_kb = true;
    coin->maxfee_kb = 100000;

    coin->has_bip44_account_path = true;
    coin->bip44_account_path = 0x8000003C;

    coin->has_decimals = true;
    coin->decimals = token->decimals;

    coin->has_contract_address = true;
    coin->contract_address.size = 20;
    memcpy((char *)&coin->contract_address.bytes[0], token->address,
           sizeof(coin->contract_address.bytes));

    coin->has_gas_limit = true;
    coin->gas_limit.size = 32;
    memcpy((char *)&coin->gas_limit.bytes[0],
           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xe8\x48",
           sizeof(coin->gas_limit.bytes));

    coin->has_curve_name = true;
    strncpy(&coin->curve_name[0], "secp256k1", sizeof(coin->curve_name));
}

void fsm_msgGetCoinTable(GetCoinTable *msg) {
    RESP_INIT(CoinTable);

    CHECK_PARAM(msg->has_start == msg->has_end,
                "Incorrect GetCoinTable parameters");

    resp->has_chunk_size = true;
    resp->chunk_size = sizeof(resp->table) / sizeof(resp->table[0]);

    if (msg->has_start && msg->has_end) {
        if (COINS_COUNT + TOKENS_COUNT <= msg->start ||
            COINS_COUNT + TOKENS_COUNT < msg->end || msg->end < msg->start ||
            resp->chunk_size < msg->end - msg->start) {
            fsm_sendFailure(FailureType_Failure_Other,
                            "Incorrect GetCoinTable parameters");
            layout_home();
            return;
        }
    }

    resp->has_num_coins = true;
    resp->num_coins = COINS_COUNT + TOKENS_COUNT;

    if (msg->has_start && msg->has_end) {
        resp->table_count = msg->end - msg->start;

        for (size_t i = 0; i < msg->end - msg->start; i++) {
            if (msg->start + i < COINS_COUNT) {
                resp->table[i] = coins[msg->start + i];
            } else if (msg->start + i - COINS_COUNT < TOKENS_COUNT) {
                coin_from_token(&resp->table[i],
                                &tokens[msg->start + i - COINS_COUNT]);
            }
        }
    }

    msg_write(MessageType_MessageType_CoinTable, resp);
}

void fsm_msgPing(Ping *msg) {
    RESP_INIT(Success);

    if (msg->has_button_protection && msg->button_protection)
        if (!confirm(ButtonRequestType_ButtonRequest_Ping, "Ping", "%s",
                     msg->message)) {
            fsm_sendFailure(FailureType_Failure_ActionCancelled,
                            "Ping cancelled");
            layout_home();
            return;
        }

    if (msg->has_pin_protection && msg->pin_protection) {
        CHECK_PIN
    }

    if (msg->has_passphrase_protection && msg->passphrase_protection) {
        if (!passphrase_protect()) {
            fsm_sendFailure(FailureType_Failure_ActionCancelled,
                            "Ping cancelled");
            layout_home();
            return;
        }
    }

    if (msg->has_message) {
        resp->has_message = true;
        memcpy(&(resp->message), &(msg->message), sizeof(resp->message));
    }

    msg_write(MessageType_MessageType_Success, resp);
    layout_home();
}

void fsm_msgChangePin(ChangePin *msg) {
    bool removal = msg->has_remove && msg->remove;
    bool confirmed = false;

    if (removal) {
        if (storage_hasPin()) {
            confirmed =
                confirm(ButtonRequestType_ButtonRequest_RemovePin, "Remove PIN",
                        "Do you want to remove PIN protection?");
        } else {
            fsm_sendSuccess("PIN removed");
            return;
        }
    } else {
        if (storage_hasPin())
            confirmed =
                confirm(ButtonRequestType_ButtonRequest_ChangePin, "Change PIN",
                        "Do you want to change your PIN?");
        else
            confirmed =
                confirm(ButtonRequestType_ButtonRequest_CreatePin, "Create PIN",
                        "Do you want to add PIN protection?");
    }

    if (!confirmed) {
        fsm_sendFailure(
            FailureType_Failure_ActionCancelled,
            removal ? "PIN removal cancelled" : "PIN change cancelled");
        layout_home();
        return;
    }

    CHECK_PIN_TXSIGN

    if (removal) {
        storage_setPin("");
        storage_commit();
        fsm_sendSuccess("PIN removed");
    } else {
        session_cachePin("");
        if (change_pin()) {
            storage_commit();
            fsm_sendSuccess("PIN changed");
        }
    }

    layout_home();
}

void fsm_msgWipeDevice(WipeDevice *msg) {
    (void)msg;

	if (!confirm(ButtonRequestType_ButtonRequest_WipeDevice, "WIPE DEVICE",
                 "Do you want to erase your private keys and settings?")) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled, "Wipe cancelled");
        layout_home();
        return;
    }

    /* Wipe device */
    storage_reset();
    storage_resetUuid();
    storage_commit();

    fsm_sendSuccess("Device wiped");
    layout_home();
}

void fsm_msgFirmwareErase(FirmwareErase *msg) {
    (void)msg;
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in bootloader mode");
}

void fsm_msgFirmwareUpload(FirmwareUpload *msg) {
    (void)msg;
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in bootloader mode");
}

void fsm_msgGetEntropy(GetEntropy *msg) {
    if (!confirm(ButtonRequestType_ButtonRequest_GetEntropy, "Generate Entropy",
                 "Do you want to generate and return entropy using the "
                 "hardware RNG?")) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled,
                        "Entropy cancelled");
        layout_home();
        return;
    }

    RESP_INIT(Entropy);
    uint32_t len = msg->size;

    if (len > ENTROPY_BUF) {
        len = ENTROPY_BUF;
    }

    resp->entropy.size = len;
    random_buffer(resp->entropy.bytes, len);
    msg_write(MessageType_MessageType_Entropy, resp);
    layout_home();
}

void fsm_msgLoadDevice(LoadDevice *msg) {
    CHECK_NOT_INITIALIZED

    if (!confirm_load_device(msg->has_node)) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled, "Load cancelled");
        layout_home();
        return;
    }

    if (msg->has_mnemonic && !(msg->has_skip_checksum && msg->skip_checksum)) {
        if (!mnemonic_check(msg->mnemonic)) {
            fsm_sendFailure(FailureType_Failure_ActionCancelled,
                            "Mnemonic with wrong checksum provided");
            layout_home();
            return;
        }
    }

    storage_loadDevice(msg);

    storage_commit();
    fsm_sendSuccess("Device loaded");
    layout_home();
}

void fsm_msgResetDevice(ResetDevice *msg) {
    CHECK_NOT_INITIALIZED

    reset_init(
        msg->has_display_random && msg->display_random,
        msg->has_strength ? msg->strength : 128,
        msg->has_passphrase_protection && msg->passphrase_protection,
        msg->has_pin_protection && msg->pin_protection,
        msg->has_language ? msg->language : 0, msg->has_label ? msg->label : 0,
        msg->has_no_backup ? msg->no_backup : false,
        msg->has_auto_lock_delay_ms ? msg->auto_lock_delay_ms
                                    : STORAGE_DEFAULT_SCREENSAVER_TIMEOUT);
}

void fsm_msgEntropyAck(EntropyAck *msg) {
    if (msg->has_entropy) {
        reset_entropy(msg->entropy.bytes, msg->entropy.size);
    } else {
        reset_entropy(0, 0);
    }
}

void fsm_msgCancel(Cancel *msg) {
    (void)msg;
    recovery_abort(true);
    signing_abort();
    ethereum_signing_abort();
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Aborted");
}

void fsm_msgApplySettings(ApplySettings *msg) {
    if (msg->has_label) {
        if (!confirm(
                ButtonRequestType_ButtonRequest_ChangeLabel, "Change Label",
                "Do you want to change the label to \"%s\"?", msg->label)) {
            goto apply_settings_cancelled;
        }
    }

    if (msg->has_language) {
        if (!confirm(ButtonRequestType_ButtonRequest_ChangeLanguage,
                     "Change Language",
                     "Do you want to change the language to %s?",
                     msg->language)) {
            goto apply_settings_cancelled;
        }
    }

    if (msg->has_use_passphrase) {
        if (msg->use_passphrase) {
            if (!confirm(ButtonRequestType_ButtonRequest_EnablePassphrase,
                         "Enable Passphrase",
                         "Do you want to enable BIP39 passphrases?")) {
                goto apply_settings_cancelled;
            }
        } else {
            if (!confirm(ButtonRequestType_ButtonRequest_DisablePassphrase,
                         "Disable Passphrase",
                         "Do you want to disable BIP39 passphrases?")) {
                goto apply_settings_cancelled;
            }
        }
    }

    if (msg->has_auto_lock_delay_ms) {
        if (!confirm(ButtonRequestType_ButtonRequest_AutoLockDelayMs,
                     "Change auto-lock delay",
                     "Do you want to set the auto-lock delay to %" PRIu32
                     " seconds?",
                     msg->auto_lock_delay_ms / 1000)) {
            goto apply_settings_cancelled;
        }
    }

    if (msg->has_u2f_counter) {
        if (!confirm(ButtonRequestType_ButtonRequest_U2FCounter,
                     "Set U2F Counter",
                     "Do you want to set the U2F Counter to %" PRIu32 "?",
                     msg->u2f_counter)) {
            goto apply_settings_cancelled;
        }
    }

    if (!msg->has_label && !msg->has_language && !msg->has_use_passphrase &&
        !msg->has_auto_lock_delay_ms && !msg->has_u2f_counter) {
        fsm_sendFailure(FailureType_Failure_SyntaxError, "No setting provided");
        return;
    }

    CHECK_PIN

    if (msg->has_label) {
        storage_setLabel(msg->label);
    }

    if (msg->has_language) {
        storage_setLanguage(msg->language);
    }

    if (msg->has_use_passphrase) {
        storage_setPassphraseProtected(msg->use_passphrase);
    }

    if (msg->has_auto_lock_delay_ms) {
        storage_setAutoLockDelayMs(msg->auto_lock_delay_ms);
    }

    if (msg->has_u2f_counter) {
        storage_setU2FCounter(msg->u2f_counter);
    }

    storage_commit();

    fsm_sendSuccess("Settings applied");
    layout_home();
    return;

apply_settings_cancelled:
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Apply settings cancelled");
    layout_home();
    return;
}

void fsm_msgRecoveryDevice(RecoveryDevice *msg) {
    CHECK_NOT_INITIALIZED

    if (msg->has_use_character_cipher &&
        msg->use_character_cipher == true)  // recovery via character cipher
    {
        recovery_cipher_init(
            msg->has_passphrase_protection && msg->passphrase_protection,
            msg->has_pin_protection && msg->pin_protection,
            msg->has_language ? msg->language : 0,
            msg->has_label ? msg->label : 0,
            msg->has_enforce_wordlist ? msg->enforce_wordlist : false,
            msg->has_auto_lock_delay_ms ? msg->auto_lock_delay_ms
                                        : STORAGE_DEFAULT_SCREENSAVER_TIMEOUT);
    } else  // legacy way of recovery
    {
        recovery_init(
            msg->has_word_count ? msg->word_count : 12,
            msg->has_passphrase_protection && msg->passphrase_protection,
            msg->has_pin_protection && msg->pin_protection,
            msg->has_language ? msg->language : 0,
            msg->has_label ? msg->label : 0,
            msg->has_enforce_wordlist ? msg->enforce_wordlist : false,
            msg->has_auto_lock_delay_ms ? msg->auto_lock_delay_ms
                                        : STORAGE_DEFAULT_SCREENSAVER_TIMEOUT);
    }
}

void fsm_msgWordAck(WordAck *msg) { recovery_word(msg->word); }

void fsm_msgCharacterAck(CharacterAck *msg) {
    if (msg->has_delete && msg->del) {
        recovery_delete_character();
    } else if (msg->has_done && msg->done) {
        recovery_cipher_finalize();
    } else {
        recovery_character(msg->character);
    }
}

void fsm_msgApplyPolicies(ApplyPolicies *msg) {
    CHECK_PARAM(msg->policy_count > 0, "No policies provided");

    for (size_t i = 0; i < msg->policy_count; ++i) {
        CHECK_PARAM(msg->policy[i].has_policy_name,
                    "Incorrect ApplyPolicies parameters");
        CHECK_PARAM(msg->policy[i].has_enabled,
                    "Incorrect ApplyPolicies parameters");
    }

    for (size_t i = 0; i < msg->policy_count; ++i) {
        RESP_INIT(ButtonRequest);
        resp->has_code = true;
        resp->code = ButtonRequestType_ButtonRequest_ApplyPolicies;
        resp->has_data = true;

        strlcpy(resp->data, msg->policy[i].policy_name, sizeof(resp->data));

        bool enabled = msg->policy[i].enabled;
        strlcat(resp->data, enabled ? ":Enable" : ":Disable",
                sizeof(resp->data));

        if (!confirm_with_custom_button_request(
                resp, enabled ? "Enable Policy" : "Disable Policy",
                "Do you want to %s %s policy?", enabled ? "enable" : "disable",
                msg->policy[i].policy_name)) {
            fsm_sendFailure(FailureType_Failure_ActionCancelled,
                            "Apply policies cancelled");
            layout_home();
            return;
        }
    }

    CHECK_PIN

    for (size_t i = 0; i < msg->policy_count; ++i) {
        if (!storage_setPolicy(msg->policy[i].policy_name,
                               msg->policy[i].enabled)) {
            fsm_sendFailure(FailureType_Failure_ActionCancelled,
                            "Policies could not be applied");
            layout_home();
            return;
        }
    }

    storage_commit();

    fsm_sendSuccess("Policies applied");
    layout_home();
}
