/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pvsr.h"

/*
private struct PVSR
{
    public StageEffect[] StageEffects;
    public Effect[] EffectList;
    public string[] EMCSList;
    public StageEffect?[] StageChangeEffects;
    public AET[] AETs;

    public struct AET
    {
        public ushort U48;
        public ushort U4A;
        public ushort U4C;
        public ushort U4E;
        public ushort U50;
        public ushort U52;
        public ushort U54;
        public ushort U56;
        public ushort U58;
        public ushort U5A;
        public ushort U5C;
        public ushort U5E;
        public ushort U60;

        public string SetName;
        public AETEntry[]    FrontList;
        public AETEntry[] FrontLowList;
        public AETEntry[]     BackList;
        public Sub1? Sub1Data;
        public Sub2? Sub2AData;
        public Sub2? Sub2BData;
        public Sub2? Sub2CData;
        public Sub2? Sub2DData;
        public AETEntry[]    Unk03List;
        public AETEntry[]    Unk04List;

        public override string ToString() =>
            $"(Set Name: {SetName}; Front List Count: {(FrontList != null ? FrontList.Length : 0)}"
            + $"; Front Low List Count: {(FrontLowList != null ? FrontLowList.Length : 0)}"
            + $"; Back List Count: {(BackList != null ? BackList.Length : 0)}"
            + $"; U48: {U48}; U4A: {U4A}; U4C: {U4C}; U4E: {U4E}; U4C: {U4C}; U50: {U50}; U52: {U52}"
            + $"; U54: {U54}; U56: {U56}; U58: {U58}; U5A: {U5A}; U5E: {U5E}; U60: {U60}"
            + $"; Unk 3 List Count: {(Unk03List != null ? Unk03List.Length : 0)}"
            + $"; Unk 4 List Count: {(Unk04List != null ? Unk04List.Length : 0)}";

        public struct Sub1
        {
            public string Name;
            public uint Hash;
            public ushort Unk2;

            public override string ToString() =>
                $"(Name: {Name}; Hash: {Hash:X08}; Unk 2: {Unk2})";
        }

        public struct Sub2
        {
            public ushort U00;
            public ushort U02;
            public ushort U04;
            public ushort U06;
            public ushort U08;
            public ushort U0A;
            public ushort U0C;

            public override string ToString() =>
                $"(U00: {U00}; U02: {U02}; U04: {U04}; U06: {U06}; U08: {U08}; U0A: {U0A})";
        }
    }

    public struct AETEntry
    {
        public string Name;
        public uint Hash;
        public float BrightScale;

        public override string ToString() =>
            $"(Name: {Name}; Hash: {Hash:X08}; Bright Scale: {BrightScale})";
    }

    public struct A3DAEntry
    {
        public string Name;
        public uint Hash;

        public override string ToString() =>
            $"(Name: {Name}; Hash: {Hash:X08})";
    }

    public struct Effect
    {
        public string Name;
        public float Emission;

        public override string ToString() =>
            $"(Name: {Name}; Emission: {Emission})";
    }

    public struct Glitter
    {
        public string Name;
        public byte Unk1;

        public override string ToString() =>
            $"(Name: {Name}; Unk 1: {Unk1})";
    }

    public struct StageEffect
    {
        public A3DAEntry[] A3DAs;
        public Glitter[] Glitters;

        public override string ToString() =>
            $"A3DAs Count: {(A3DAs != null ? A3DAs.Length : 0)}"
            + $"; Effects Count: {(Glitters != null ? Glitters.Length : 0)})";
    }
}

private static PVSR PVSRReader(string file)
{
    int i, j;
    KKdBaseLib.F2.Struct st = File.ReadAllBytes(file + ".pvsr").RSt();

    PVSR pvsr = default;
    Stream s = File.OpenReader(st.Data);
    s.IsBE = st.Header.UseBigEndian;
    s.IsX = true;

    int x00 = s.RI32E();
    byte x04 = s.RU8();
    byte x05 = s.RU8();
    byte x06 = s.RU8();
    byte x07 = s.RU8();
    byte stageEffectsCount = s.RU8();
    byte         aetsCount = s.RU8();
    byte x0A = s.RU8();
    byte x0B = s.RU8();
    byte x0C = s.RU8();
    byte x0D = s.RU8();
    byte x0E = s.RU8();
    byte x0F = s.RU8();
    long                    x10 = s.RI64E();
    long     stageEffectsOffset = s.RI64E();
    long stageEffectsListOffset = s.RI64E();
    long             aetsOffset = s.RI64E();

    s.PI64 = x10;
    byte UnknownsCount = s.RU8();
    byte  EffectsCount = s.RU8();
    byte    EMCSsCount = s.RU8();
    s.A(0x08);
    long UnknownOffset = s.RI64E();
    long EffectsOffset = s.RI64E();
    long   EMCSsOffset = s.RI64E();

    s.PI64 = EffectsOffset;
    pvsr.EffectList = new PVSR.Effect[EffectsCount];
    for (i = 0; i < EffectsCount; i++)
    {
        ref PVSR.Effect effect = ref pvsr.EffectList[i];
        effect.Name = s.RSaO();
        effect.Emission = s.RF32E();
        s.A(0x08);
    }

    s.PI64 = EMCSsOffset;
    pvsr.EMCSList = new string[EMCSsCount];
    for (i = 0; i < EMCSsCount; i++)
        pvsr.EMCSList[i] = s.RSaO();

    s.PI64 = stageEffectsOffset;
    pvsr.StageEffects = new PVSR.StageEffect[stageEffectsCount];
    for (i = 0; i < stageEffectsCount; i++)
        pvsr.StageEffects[i] = readStageEffect();

    s.PI64 = stageEffectsListOffset;
    pvsr.StageChangeEffects = new PVSR.StageEffect?[0x100];
    for (i = 0; i < 0x100; i++)
    {
        long o = s.RI64E();
        if (o > 0)
            pvsr.StageChangeEffects[i] = readStageEffect(o);

    }

    s.PI64 = aetsOffset;
    pvsr.AETs = new PVSR.AET[aetsCount];
    for (i = 0; i < aetsCount; i++)
    {
        long offset = s.PI64;
        ref PVSR.AET aet = ref pvsr.AETs[i];

        long  setNameOffset = s.RI64E();
        long    frontOffset = s.RI64E();
        long frontLowOffset = s.RI64E();
        long     backOffset = s.RI64E();
        long     sub1Offset = s.RI64E();
        long    sub2AOffset = s.RI64E();
        long    sub2BOffset = s.RI64E();
        long    sub2COffset = s.RI64E();
        long    sub2DOffset = s.RI64E();
        aet.U48 = s.RU16E();
        aet.U4A = s.RU16E();
        aet.U4C = s.RU16E();
        aet.U4E = s.RU16E();
        aet.U50 = s.RU16E();
        aet.U52 = s.RU16E();
        aet.U54 = s.RU16E();
        aet.U56 = s.RU16E();
        aet.U58 = s.RU16E();
        aet.U5A = s.RU16E();
        aet.U5C = s.RU16E();
        aet.U5E = s.RU16E();
        aet.U60 = s.RU16E();
        byte    frontCount = s.RU8();
        byte frontLowCount = s.RU8();
        byte     backCount = s.RU8();
        s.A(0x08);
        long O68 = 0;
        long O70 = 0;
        byte U78 = 0;
        byte U79 = 0;
        if ((x00 & 0x100) != 0)
        {
            O68 = s.RI64E();
            O70 = s.RI64E();
            U78 = s.RU8();
            U79 = s.RU8();
            s.A(0x08);
        }

        aet.SetName = s.RSaO(setNameOffset);

        s.PI64 = frontOffset;
        aet.FrontList = new PVSR.AETEntry[frontCount];
        for (j = 0; j < frontCount; j++)
            aet.FrontList[j] = readAETEntry();

        s.PI64 = frontLowOffset;
        aet.FrontLowList = new PVSR.AETEntry[frontLowCount];
        for (j = 0; j < frontLowCount; j++)
            aet.FrontLowList[j] = readAETEntry();

        s.PI64 = backOffset;
        aet.BackList = new PVSR.AETEntry[backCount];
        for (j = 0; j < backCount; j++)
            aet.BackList[j] = readAETEntry();

        aet.Sub1Data = readAETSub1(sub1Offset);
        aet.Sub2AData = readAETSub2(sub2AOffset);
        aet.Sub2BData = readAETSub2(sub2BOffset);
        aet.Sub2CData = readAETSub2(sub2COffset);
        aet.Sub2DData = readAETSub2(sub2DOffset);

        if (O68 > 0)
        {
            s.PI64 = O68;
            aet.Unk03List = new PVSR.AETEntry[U78];
            for (j = 0; j < U78; j++)
                aet.Unk03List[j] = readAETEntry();
        }

        if (O70 > 0)
        {
            s.PI64 = O70;
            aet.Unk04List = new PVSR.AETEntry[U79];
            for (j = 0; j < U79; j++)
                aet.Unk04List[j] = readAETEntry();
        }
        s.PI64 = offset + ((x00 & 0x100) != 0 ? 0x80 : 0x68);
    }
    s.C();
    return pvsr;

    PVSR.StageEffect readStageEffect(long o = 0)
    {
        long offset = s.PI64;
        if (o > 0)
            s.PI64 = o;
        PVSR.StageEffect eff = default;
        byte U00 = s.RU8();
        byte U01 = s.RU8();
        byte U02 = s.RU8();
        byte U03 = s.RU8();
        byte U04 = s.RU8();
        s.A(0x08);
        long    A3DAsOffset = s.RI64E();
        long GlittersOffset = s.RI64E();

        int    a3dasCount;
        int glittersCount;
        if (U04 != 0xFF)
        {
               a3dasCount = U00;
            glittersCount = U01;
        }
        else
        {
               a3dasCount = U01;
            glittersCount = U02;
        }

        if (A3DAsOffset > 0)
        {
            s.PI64 = A3DAsOffset;
            eff.A3DAs = new PVSR.A3DAEntry[a3dasCount];
            for (int i = 0; i < a3dasCount; i++)
            {
                PVSR.A3DAEntry entry = default;
                entry.Name = s.RSaO();
                entry.Hash = s.RU32E();
                s.A(0x08);
                eff.A3DAs[i] = entry;
            }
        }

        if (GlittersOffset > 0)
        {
            s.PI64 = GlittersOffset;
            eff.Glitters = new PVSR.Glitter[glittersCount];
            for (int i = 0; i < glittersCount; i++)
                eff.Glitters[i] = readGlitter();
        }

        if (o > 0)
            s.PI64 = offset;
        else
            s.PI64 = offset + 0x20;
        return eff;
    }

    PVSR.AETEntry readAETEntry()
    {
        PVSR.AETEntry entry = default;
        entry.Name = s.RSaO();
        entry.Hash = s.RU32E();
        entry.BrightScale = s.RF32E();
        return entry;
    }

    PVSR.AET.Sub1? readAETSub1(long offset)
    {
        if (offset <= 0)
            return default;

        s.PI64 = offset;
        PVSR.AET.Sub1 sub = default;
        sub.Name = s.RSaO();
        sub.Hash = s.RU32E();
        sub.Unk2 = s.RU16E();
        s.A(0x08);
        return sub;
    }

    PVSR.AET.Sub2? readAETSub2(long offset)
    {
        if (offset <= 0)
            return default;

        s.PI64 = offset;
        PVSR.AET.Sub2 sub = default;
        sub.U00 = s.RU16E();
        sub.U02 = s.RU16E();
        sub.U04 = s.RU16E();
        sub.U06 = s.RU16E();
        sub.U08 = s.RU16E();
        sub.U0A = s.RU16E();
        sub.U0C = s.RU16E();
        s.A(0x08);
        return sub;
    }

    PVSR.Glitter readGlitter()
    {
        PVSR.Glitter eff = default;
        eff.Name = s.RSaO();
        eff.Unk1 = s.RU8();
        s.A(0x08);
        return eff;
    }
}

private static void PVSRMsgPackWriter(string file, PVSR pvsr)
{
    int i, j;
    MsgPack _PVSR = new MsgPack("PVSR");

    int stageEffectsCount = pvsr.StageEffects != null ? pvsr.StageEffects.Length : 0;
    MsgPack stgEffs = new MsgPack(stageEffectsCount, "StageEffects");
    for (i = 0; i < stageEffectsCount; i++)
        stgEffs[i] = writeStageEffect(pvsr.StageEffects[i]);
    if (stageEffectsCount > 0)
        _PVSR.Add(stgEffs);

    int effectsCount = pvsr.EffectList != null ? pvsr.EffectList.Length : 0;
    MsgPack effectList = new MsgPack(effectsCount, "EffectList");
    for (i = 0; i < effectsCount; i++)
    {
        ref PVSR.Effect effect = ref pvsr.EffectList[i];
        effectList[i] = MsgPack.New.Add("Name", effect.Name).Add("Emission", effect.Emission);
    }
    if (effectsCount > 0)
        _PVSR.Add(effectList);

    int emcssCount = pvsr.EMCSList != null ? pvsr.EMCSList.Length : 0;
    MsgPack emcsList = new MsgPack(emcssCount, "EMCSList");
    for (i = 0; i < emcssCount; i++)
        emcsList[i] = pvsr.EMCSList[i];
    if (emcssCount > 0)
        _PVSR.Add(emcsList);

    MsgPack stgChgEffs = new MsgPack(0x10, "StageChangeEffects");
    for (i = 0; i < 0x10; i++)
    {
        MsgPack stgChgEff = new MsgPack(0x10);
        for (j = 0; j < 0x10; j++)
        {
            if (pvsr.StageChangeEffects[i * 0x10 + j].HasValue)
                stgChgEff[j] = writeStageEffect(pvsr.StageChangeEffects[i * 0x10 + j].Value);
            else
                stgChgEff[j] = default;
        }
        stgChgEffs[i] = stgChgEff;
    }
    _PVSR.Add(stgChgEffs);

    int aetsCount = pvsr.AETs != null ? pvsr.AETs.Length : 0;
    MsgPack aets = new MsgPack(aetsCount, "AETs");
    for (i = 0; i < aetsCount; i++)
    {
        ref PVSR.AET aet = ref pvsr.AETs[i];

        MsgPack a = MsgPack.New.Add("SetName", aet.SetName);
        int frontCount = aet.FrontList != null ? aet.FrontList.Length : 0;
        MsgPack frontList = new MsgPack(frontCount, "FrontList");
        for (j = 0; j < frontCount; j++)
            frontList[j] = writeAETEntry(aet.FrontList[j]);
        if (frontCount > 0)
            a.Add(frontList);

        int frontLowCount = aet.FrontLowList != null ? aet.FrontLowList.Length : 0;
        MsgPack frontLowList = new MsgPack(frontLowCount, "FrontLowList");
        for (j = 0; j < frontLowCount; j++)
            frontLowList[j] = writeAETEntry(aet.FrontLowList[j]);
        if (frontLowCount > 0)
            a.Add(frontLowList);

        int backCount = aet.BackList != null ? aet.BackList.Length : 0;
        MsgPack backList = new MsgPack(backCount, "BackList");
        for (j = 0; j < backCount; j++)
            backList[j] = writeAETEntry(aet.BackList[j]);
        if (backCount > 0)
            a.Add(backList);

        a.Add("U48", aet.U48);
        a.Add("U4A", aet.U4A);
        a.Add("U4C", aet.U4C);
        a.Add("U4E", aet.U4E);
        a.Add("U50", aet.U50);
        a.Add("U52", aet.U52);
        a.Add("U54", aet.U54);
        a.Add("U56", aet.U56);
        a.Add("U58", aet.U58);
        a.Add("U5A", aet.U5A);
        a.Add("U5C", aet.U5C);
        a.Add("U5E", aet.U5E);
        a.Add("U60", aet.U60);

        MsgPack temp;
        if ((temp = writeAETSub1(aet.Sub1Data, "Sub1")).NotNull)
            a.Add(temp);
        if ((temp = writeAETSub2(aet.Sub2AData, "Sub2A")).NotNull)
            a.Add(temp);
        if ((temp = writeAETSub2(aet.Sub2BData, "Sub2B")).NotNull)
            a.Add(temp);
        if ((temp = writeAETSub2(aet.Sub2CData, "Sub2C")).NotNull)
            a.Add(temp);
        if ((temp = writeAETSub2(aet.Sub2DData, "Sub2D")).NotNull)
            a.Add(temp);

        int unk03Count = aet.FrontList != null ? aet.FrontList.Length : 0;
        MsgPack unk03List = new MsgPack(unk03Count, "Unk04List");
        for (j = 0; j < unk03Count; j++)
            unk03List[j] = writeAETEntry(aet.Unk03List[j]);
        if (unk03Count > 0)
            _PVSR.Add(unk03List);

        int unk04Count = aet.Unk04List != null ? aet.Unk04List.Length : 0;
        MsgPack unk04List = new MsgPack(unk04Count, "Unk04List");
        for (j = 0; j < unk04Count; j++)
            unk04List[j] = writeAETEntry(aet.Unk04List[j]);
        if (unk04Count > 0)
            a.Add(unk04List);
        aets[i] = a;
    }
    if (aetsCount > 0)
        _PVSR.Add(aets);

    _PVSR.Write(false, true, file, json);

    MsgPack writeStageEffect(PVSR.StageEffect eff, string name = null)
    {
        MsgPack effect = new MsgPack(name);
        int a3dasCount = eff.A3DAs != null ? eff.A3DAs.Length : 0;
        MsgPack a3das = new MsgPack(a3dasCount, "A3DAs");
        for (int i = 0; i < a3dasCount; i++)
            a3das[i] = eff.A3DAs[i].Name;
        if (a3dasCount > 0)
            effect.Add(a3das);

        int glittersCount = eff.Glitters != null ? eff.Glitters.Length : 0;
        MsgPack glitters = new MsgPack(glittersCount, "Glitters");
        for (int i = 0; i < glittersCount; i++)
            glitters[i] = writeGlitter(eff.Glitters[i]);
        if (glittersCount > 0)
            effect.Add(glitters);

        if (a3dasCount > 0 || glittersCount > 0)
            return effect;
        else
            return default;
    }

    MsgPack writeAETEntry(PVSR.AETEntry entry, string name = null) =>
        new MsgPack(name).Add("Name", entry.Name).Add("BrightScale", entry.BrightScale);

    MsgPack writeAETSub1(PVSR.AET.Sub1? sub, string name = null)
    {
        if (!sub.HasValue)
            return default;

        PVSR.AET.Sub1 s = sub.Value;
        return new MsgPack(name).Add("Name", s.Name).Add("Unk2", s.Unk2);
    }

    MsgPack writeAETSub2(PVSR.AET.Sub2? sub, string name = null)
    {
        if (!sub.HasValue)
            return default;

        PVSR.AET.Sub2 s = sub.Value;
        return new MsgPack(name)
            .Add("U00", s.U00).Add("U02", s.U02).Add("U04", s.U04).Add("U06", s.U06)
            .Add("U08", s.U08).Add("U0A", s.U0A).Add("U0C", s.U0C);
    }

    MsgPack writeGlitter(PVSR.Glitter eff, string name = null) =>
        new MsgPack(name).Add("Name", eff.Name).Add("Unk1", eff.Unk1);
}
*/
